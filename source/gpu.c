//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:13:23
//

#include <string.h>

#include "gpu.h"

void gpu_init(gpu_t *gpu, gpu_config_t *config)
{
    memset(gpu, 0, sizeof(gpu_t));

    gpu->fb_count = config->fb_count;
    gpu_resize(gpu, config->mode);

    // @note(ame): create device
    // @todo(ame): custom allocators?
    DkDeviceMaker device_maker;
    dkDeviceMakerDefaults(&device_maker); // Depth [0; 1], Origin is upper left.
    gpu->device = dkDeviceCreate(&device_maker);

    // @note(ame): allocate framebuffers
    DkImageLayoutMaker fb_maker;
    dkImageLayoutMakerDefaults(&fb_maker, gpu->device);
    fb_maker.flags = DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression;
    fb_maker.format = DkImageFormat_RGBA8_Unorm;
    fb_maker.dimensions[0] = gpu->width;
    fb_maker.dimensions[1] = gpu->height;

    DkImageLayout fb_layout;
    dkImageLayoutInitialize(&fb_layout, &fb_maker);

    u32 fb_size = dkImageLayoutGetSize(&fb_layout);
    u32 fb_align = dkImageLayoutGetAlignment(&fb_layout);
    fb_size = (fb_size + fb_align - 1) &~ (fb_align - 1);

    // @note(ame): allocate arenas
    arena_init_gpu(&gpu->swapchain_arena, fb_size * gpu->fb_count, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, gpu->device);

    // @note(ame): allocate back buffers
    DkImage const* swapchain_images[DEFAULT_GPU_FB_COUNT];
    for (i32 i = 0; i < gpu->fb_count; i++) {
        swapchain_images[i] = &gpu->fbs[i];
        dkImageInitialize(&gpu->fbs[i], &fb_layout, gpu->swapchain_arena.gpu_block, i * fb_size);
    }

    // @note(ame): swapchain
    DkSwapchainMaker swap_maker;
    dkSwapchainMakerDefaults(&swap_maker, gpu->device, nwindowGetDefault(), swapchain_images, gpu->fb_count);
    gpu->swapchain = dkSwapchainCreate(&swap_maker);

    // @note(ame): queue
    DkQueueMaker queue_maker;
    dkQueueMakerDefaults(&queue_maker, gpu->device);
    queue_maker.flags = DkQueueFlags_Graphics;
    gpu->queue = dkQueueCreate(&queue_maker);
}

void gpu_resize(gpu_t *gpu, AppletOperationMode mode)
{
    switch (mode) {
        case AppletOperationMode_Console: {
            gpu->width = DOCKED_WIDTH;
            gpu->height = DOCKED_HEIGHT;
            break;
        };
        case AppletOperationMode_Handheld: {
            gpu->width = UNDOCKED_WIDTH;
            gpu->height = UNDOCKED_HEIGHT;
            break;
        };
    }
}

void gpu_exit(gpu_t *gpu)
{
    dkQueueWaitIdle(gpu->queue);

    dkQueueDestroy(gpu->queue);
    dkSwapchainDestroy(gpu->swapchain);
    arena_free(&gpu->swapchain_arena);
    dkDeviceDestroy(gpu->device);
}
