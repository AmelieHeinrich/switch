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
#include "arena.h"

#define DEFAULT_GPU_FB_COUNT 2

#define DOCKED_WIDTH 1920
#define DOCKED_HEIGHT 1080
#define UNDOCKED_WIDTH 1280
#define UNDOCKED_HEIGHT 720

#define CMD_ARENA_SIZE KILOBYTES(16)

typedef struct gpu_config_t {
    AppletOperationMode mode;
    u32 fb_count;
} gpu_config_t;

typedef struct gpu_t {
    // @note(ame): config
    u32 width;
    u32 height;
    u32 fb_count;

    // @note(ame): internals
    DkDevice device;
    DkQueue queue;
    DkImage fbs[DEFAULT_GPU_FB_COUNT];
    DkSwapchain swapchain;
    
    // @note(ame): arenas
    arena_t swapchain_arena;
} gpu_t;

void gpu_init(gpu_t *gpu, gpu_config_t *config);
void gpu_resize(gpu_t *gpu, AppletOperationMode mode);
void gpu_exit(gpu_t *gpu);

#endif
