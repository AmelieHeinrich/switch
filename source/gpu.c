//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:13:23
//

#include "gpu.h"

void gpu_init(gpu_t *gpu, gpu_config_t *config)
{
    gpu->fb_count = config->fb_count;
    gpu_resize(gpu, config->mode);
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

}
