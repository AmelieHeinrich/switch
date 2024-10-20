//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:08:02
//

#ifndef GPU_H_
#define GPU_H_

#include <switch.h>

#include "core.h"

#define DEFAULT_GPU_FB_COUNT 2
#define DOCKED_WIDTH 1920
#define DOCKED_HEIGHT 1080
#define UNDOCKED_WIDTH 1280
#define UNDOCKED_HEIGHT 720

typedef struct gpu_config_t {
    AppletOperationMode mode;
    u32 fb_count;
} gpu_config_t;

typedef struct gpu_t {
    u32 width;
    u32 height;
    u32 fb_count;
} gpu_t;

void gpu_init(gpu_t *gpu, gpu_config_t *config);
void gpu_resize(gpu_t *gpu, AppletOperationMode mode);
void gpu_exit(gpu_t *gpu);

#endif
