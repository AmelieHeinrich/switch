//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:08:02
//

#ifndef GPU_H_
#define GPU_H_

#include <switch.h>
#include <deko3d.h>

#include "core.h"
#include "heap.h"
#include "cmd_mem_ring.h"
#include "shader_loader.h"

#define DEFAULT_GPU_FB_COUNT 2

#define DOCKED_WIDTH 1920
#define DOCKED_HEIGHT 1080
#define UNDOCKED_WIDTH 1280
#define UNDOCKED_HEIGHT 720

#define CMD_ARENA_SIZE MEGABYTES(64)
#define DATA_ARENA_SIZE MEGABYTES(64)
#define IMAGE_ARENA_SIZE MEGABYTES(64)
#define UNIFORM_ARENA_SIZE MEGABYTES(64)

typedef struct gpu_config_t {
    AppletOperationMode mode;
} gpu_config_t;

typedef struct frame_t {
    DkImageView backbuffer_view;
    DkCmdBuf cmd_buf;
    u32 frame_idx;
} frame_t;

typedef struct gpu_t {
    // @note(ame): config
    u32 width;
    u32 height;
    u32 fb_count;
    i32 curr_frame;

    // @note(ame): internals
    DkDevice device;
    DkQueue queue;
    DkSwapchain swapchain;
    shader_loader_t shader_loader;

    // @note(ame): frame dependent
    cmd_mem_ring_t cmd_ring;
    DkImage fbs[DEFAULT_GPU_FB_COUNT];
    DkImageView image_views[DEFAULT_GPU_FB_COUNT];
    DkCmdBuf cmd_bufs[DEFAULT_GPU_FB_COUNT];

    // @note(ame): heaps
    heap_t swapchain_heap;
    heap_t cmd_heap;
    heap_t uniform_heap; // for uniform buffers that need their stupid alignment
    heap_t data_heap; // for single use command buffers, vertex/index buffers, and CPU bitmaps
    heap_t image_heap; // for buffers, textures and whatnot
} gpu_t;

void gpu_init(gpu_t *gpu, gpu_config_t *config);
void gpu_exit(gpu_t *gpu);
void gpu_resize(gpu_t *gpu, AppletOperationMode mode);
void gpu_wait(gpu_t *gpu);

frame_t gpu_begin(gpu_t *gpu);
void gpu_end(gpu_t *gpu, frame_t *frame);
void gpu_present(gpu_t *gpu);

#endif
