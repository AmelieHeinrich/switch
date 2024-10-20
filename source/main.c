//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 16:16:57
//

#if 1
#include "app.h"

int main(int argc, char* argv[])
{
    app_config_t config = {
        .print_to_fb = true,
        .load_user_icon = false,
        .redirect_stdio = true
    };

    app_t app;
    app_init(&app, &config);

    app_run(&app);
    
    app_exit(&app);
    return 0;
}
#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>
#include <deko3d.h>

// Define the desired number of framebuffers
#define FB_NUM 2

// Define the desired framebuffer resolution (here we set it to 720p).
#define FB_WIDTH  1280
#define FB_HEIGHT 720

// Remove above and uncomment below for 1080p
//#define FB_WIDTH  1920
//#define FB_HEIGHT 1080

// Define the size of the memory block that will hold code
#define CODEMEMSIZE (64*1024)

// Define the size of the memory block that will hold command lists
#define CMDMEMSIZE (16*1024)

static DkDevice g_device;
static DkMemBlock g_framebufferMemBlock;
static DkImage g_framebuffers[FB_NUM];
static DkSwapchain g_swapchain;

static DkMemBlock g_cmdbufMemBlock;
static DkCmdBuf g_cmdbuf;
static DkCmdList g_cmdsBindFramebuffer[FB_NUM];
static DkCmdList g_cmdsRender;

static DkQueue g_renderQueue;

// This function creates all the necessary graphical resources.
static void graphicsInitialize(void)
{
    // Create the device, which is the root object
    DkDeviceMaker deviceMaker;
    dkDeviceMakerDefaults(&deviceMaker);
    g_device = dkDeviceCreate(&deviceMaker);

    // Calculate layout for the framebuffers
    DkImageLayoutMaker imageLayoutMaker;
    dkImageLayoutMakerDefaults(&imageLayoutMaker, g_device);
    imageLayoutMaker.flags = DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression;
    imageLayoutMaker.format = DkImageFormat_RGBA8_Unorm;
    imageLayoutMaker.dimensions[0] = FB_WIDTH;
    imageLayoutMaker.dimensions[1] = FB_HEIGHT;

    // Calculate layout for the framebuffers
    DkImageLayout framebufferLayout;
    dkImageLayoutInitialize(&framebufferLayout, &imageLayoutMaker);

    // Retrieve necessary size and alignment for the framebuffers
    uint32_t framebufferSize  = dkImageLayoutGetSize(&framebufferLayout);
    uint32_t framebufferAlign = dkImageLayoutGetAlignment(&framebufferLayout);
    framebufferSize = (framebufferSize + framebufferAlign - 1) &~ (framebufferAlign - 1);

    // Create a memory block that will host the framebuffers
    DkMemBlockMaker memBlockMaker;
    dkMemBlockMakerDefaults(&memBlockMaker, g_device, FB_NUM*framebufferSize);
    memBlockMaker.flags = DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image;
    g_framebufferMemBlock = dkMemBlockCreate(&memBlockMaker);

    // Initialize the framebuffers with the layout and backing memory we've just created
    DkImage const* swapchainImages[FB_NUM];
    for (unsigned i = 0; i < FB_NUM; i ++)
    {
        swapchainImages[i] = &g_framebuffers[i];
        dkImageInitialize(&g_framebuffers[i], &framebufferLayout, g_framebufferMemBlock, i*framebufferSize);
    }

    // Create a swapchain out of the framebuffers we've just initialized
    DkSwapchainMaker swapchainMaker;
    dkSwapchainMakerDefaults(&swapchainMaker, g_device, nwindowGetDefault(), swapchainImages, FB_NUM);
    g_swapchain = dkSwapchainCreate(&swapchainMaker);

    // Create a memory block which will be used for recording command lists using a command buffer
    dkMemBlockMakerDefaults(&memBlockMaker, g_device, CMDMEMSIZE);
    memBlockMaker.flags = DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached;
    g_cmdbufMemBlock = dkMemBlockCreate(&memBlockMaker);

    // Create a command buffer object
    DkCmdBufMaker cmdbufMaker;
    dkCmdBufMakerDefaults(&cmdbufMaker, g_device);
    g_cmdbuf = dkCmdBufCreate(&cmdbufMaker);

    // Feed our memory to the command buffer so that we can start recording commands
    dkCmdBufAddMemory(g_cmdbuf, g_cmdbufMemBlock, 0, CMDMEMSIZE);

    // Generate a command list for each framebuffer, which will bind each of them as a render target
    for (unsigned i = 0; i < FB_NUM; i ++)
    {
        DkImageView imageView;
        dkImageViewDefaults(&imageView, &g_framebuffers[i]);
        dkCmdBufBindRenderTarget(g_cmdbuf, &imageView, NULL);
        g_cmdsBindFramebuffer[i] = dkCmdBufFinishList(g_cmdbuf);
    }

    // Declare structs that will be used for binding state
    DkViewport viewport = { 0.0f, 0.0f, (float)FB_WIDTH, (float)FB_HEIGHT, 0.0f, 1.0f };
    DkScissor scissor = { 0, 0, FB_WIDTH, FB_HEIGHT };
    DkRasterizerState rasterizerState;
    DkColorState colorState;
    DkColorWriteState colorWriteState;

    // Initialize state structs with the deko3d defaults
    dkRasterizerStateDefaults(&rasterizerState);
    dkColorStateDefaults(&colorState);
    dkColorWriteStateDefaults(&colorWriteState);

    // Generate the main rendering command list
    dkCmdBufSetViewports(g_cmdbuf, 0, &viewport, 1);
    dkCmdBufSetScissors(g_cmdbuf, 0, &scissor, 1);
    dkCmdBufClearColorFloat(g_cmdbuf, 0, DkColorMask_RGBA, 0.125f, 0.294f, 0.478f, 1.0f);
    g_cmdsRender = dkCmdBufFinishList(g_cmdbuf);

    // Create a queue, to which we will submit our command lists
    DkQueueMaker queueMaker;
    dkQueueMakerDefaults(&queueMaker, g_device);
    queueMaker.flags = DkQueueFlags_Graphics;
    g_renderQueue = dkQueueCreate(&queueMaker);
}

// This function is to be called at each frame, and it is in charge of rendering.
static void graphicsUpdate(void)
{
    // Acquire a framebuffer from the swapchain (and wait for it to be available)
    int slot = dkQueueAcquireImage(g_renderQueue, g_swapchain);

    // Run the command list that binds said framebuffer as a render target
    dkQueueSubmitCommands(g_renderQueue, g_cmdsBindFramebuffer[slot]);

    // Run the main rendering command list
    dkQueueSubmitCommands(g_renderQueue, g_cmdsRender);

    // Now that we are done rendering, present it to the screen
    dkQueuePresentImage(g_renderQueue, g_swapchain, slot);
}

// This function destroys the graphical resources created by graphicsInitialize.
static void graphicsExit(void)
{
    // Make sure the rendering queue is idle before destroying anything
    dkQueueWaitIdle(g_renderQueue);

    // Destroy all the resources we've created
    dkQueueDestroy(g_renderQueue);
    dkCmdBufDestroy(g_cmdbuf);
    dkMemBlockDestroy(g_cmdbufMemBlock);
    dkSwapchainDestroy(g_swapchain);
    dkMemBlockDestroy(g_framebufferMemBlock);
    dkDeviceDestroy(g_device);
}

// Main entrypoint
int main(int argc, char* argv[])
{
    graphicsInitialize();

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    while (appletMainLoop())
    {
        padUpdate(&pad);

        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_Plus)
            break; // break in order to return to hbmenu

        graphicsUpdate();
    }

    graphicsExit();
    return 0;
}

#endif
