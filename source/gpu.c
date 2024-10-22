//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:13:23
//

#include <string.h>

#include "gpu.h"
#include "util.h"

void gpu_init(gpu_t *gpu, gpu_config_t *config)
{
    memset(gpu, 0, sizeof(gpu_t));

    gpu->fb_count = DEFAULT_GPU_FB_COUNT;
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
    fb_maker.format = DkImageFormat_BGRA8_Unorm_sRGB;
    fb_maker.dimensions[0] = gpu->width;
    fb_maker.dimensions[1] = gpu->height;

    // @note(ame): Initialize swapchain layout
    DkImageLayout fb_layout;
    dkImageLayoutInitialize(&fb_layout, &fb_maker);

    // @note(ame): Get framebuffer size & alignment
    u32 fb_size = dkImageLayoutGetSize(&fb_layout);
    u32 fb_align = dkImageLayoutGetAlignment(&fb_layout);
    fb_size = (fb_size + fb_align - 1) &~ (fb_align - 1);

    // @note(ame): allocate arenas
    heap_init(&gpu->swapchain_heap, fb_size * DEFAULT_GPU_FB_COUNT, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, gpu->device);
    heap_init(&gpu->cmd_heap, CMD_ARENA_SIZE, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, gpu->device);
    heap_init(&gpu->data_heap, DATA_ARENA_SIZE, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, gpu->device);
    heap_init(&gpu->uniform_heap, UNIFORM_ARENA_SIZE, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, gpu->device);

    // @note(ame): allocate back buffers
    DkImage const* swapchain_images[DEFAULT_GPU_FB_COUNT];
    for (i32 i = 0; i < DEFAULT_GPU_FB_COUNT; i++) {
        swapchain_images[i] = &gpu->fbs[i];
        dkImageInitialize(&gpu->fbs[i], &fb_layout, gpu->swapchain_heap.block, i * fb_size);

        DkCmdBufMaker cmd_maker;
        dkCmdBufMakerDefaults(&cmd_maker, gpu->device);
        gpu->cmd_bufs[i] = dkCmdBufCreate(&cmd_maker);

        dkImageViewDefaults(&gpu->image_views[i], &gpu->fbs[i]);
    }

    // @note(ame): swapchain creation
    DkSwapchainMaker swap_maker;
    dkSwapchainMakerDefaults(&swap_maker, gpu->device, nwindowGetDefault(), swapchain_images, DEFAULT_GPU_FB_COUNT);
    gpu->swapchain = dkSwapchainCreate(&swap_maker);

    // @note(ame): queue
    DkQueueMaker queue_maker;
    dkQueueMakerDefaults(&queue_maker, gpu->device);
    queue_maker.flags = DkQueueFlags_Graphics;
    gpu->queue = dkQueueCreate(&queue_maker);

    // @note(ame): cmd ring
    cmd_mem_ring_init(&gpu->cmd_ring, gpu->device, &gpu->cmd_heap, DEFAULT_GPU_FB_COUNT);

    // @note(ame): shader loader
    shader_loader_init(&gpu->shader_loader, gpu->device);
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

    if (!gpu->swapchain) return;

    dkQueueWaitIdle(gpu->queue);

    // @todo(ame): resize
    dkSwapchainDestroy(gpu->swapchain);

    // @note(ame): allocate framebuffers
    DkImageLayoutMaker fb_maker;
    dkImageLayoutMakerDefaults(&fb_maker, gpu->device);
    fb_maker.flags = DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression;
    fb_maker.format = DkImageFormat_BGRA8_Unorm_sRGB;
    fb_maker.dimensions[0] = gpu->width;
    fb_maker.dimensions[1] = gpu->height;

    // @note(ame): Initialize swapchain layout
    DkImageLayout fb_layout;
    dkImageLayoutInitialize(&fb_layout, &fb_maker);

    // @note(ame): Get framebuffer size & alignment
    u32 fb_size = dkImageLayoutGetSize(&fb_layout);
    u32 fb_align = dkImageLayoutGetAlignment(&fb_layout);
    fb_size = (fb_size + fb_align - 1) &~ (fb_align - 1);

    // @note(ame): recreate heaps
    heap_free(&gpu->swapchain_heap);
    heap_init(&gpu->swapchain_heap, fb_size * DEFAULT_GPU_FB_COUNT, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, gpu->device);

    // @note(ame): allocate back buffers
    DkImage const* swapchain_images[DEFAULT_GPU_FB_COUNT];
    for (i32 i = 0; i < DEFAULT_GPU_FB_COUNT; i++) {
        swapchain_images[i] = &gpu->fbs[i];
        dkImageInitialize(&gpu->fbs[i], &fb_layout, gpu->swapchain_heap.block, i * fb_size);

        dkImageViewDefaults(&gpu->image_views[i], &gpu->fbs[i]);
    }

    // @note(ame): swapchain creation
    DkSwapchainMaker swap_maker;
    dkSwapchainMakerDefaults(&swap_maker, gpu->device, nwindowGetDefault(), swapchain_images, DEFAULT_GPU_FB_COUNT);
    gpu->swapchain = dkSwapchainCreate(&swap_maker);
}

void gpu_exit(gpu_t *gpu)
{
    dkQueueWaitIdle(gpu->queue);

    shader_loader_free(&gpu->shader_loader);
    
    for (i32 i = 0; i < DEFAULT_GPU_FB_COUNT; i++) {
        dkCmdBufDestroy(gpu->cmd_bufs[i]);
    }
    cmd_mem_ring_free(&gpu->cmd_ring);
    
    dkQueueDestroy(gpu->queue);
    dkSwapchainDestroy(gpu->swapchain);

    heap_free(&gpu->uniform_heap);
    heap_free(&gpu->data_heap);
    heap_free(&gpu->cmd_heap);
    heap_free(&gpu->swapchain_heap);
    
    dkDeviceDestroy(gpu->device);
}

frame_t gpu_begin(gpu_t *gpu)
{
    gpu->curr_frame = dkQueueAcquireImage(gpu->queue, gpu->swapchain);

    cmd_mem_ring_begin(&gpu->cmd_ring, gpu->cmd_bufs[gpu->curr_frame]);

    return (frame_t){
        .backbuffer_view = gpu->image_views[gpu->curr_frame],
        .cmd_buf = gpu->cmd_bufs[gpu->curr_frame],
        .frame_idx = gpu->curr_frame
    };
}

void gpu_end(gpu_t *gpu, frame_t *frame)
{
    DkCmdList list = cmd_mem_ring_end(&gpu->cmd_ring, frame->cmd_buf);
    dkQueueSubmitCommands(gpu->queue, list);
}

void gpu_present(gpu_t *gpu)
{
    dkQueuePresentImage(gpu->queue, gpu->swapchain, gpu->curr_frame);
}
