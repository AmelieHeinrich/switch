//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 16:58:52
//

#ifndef APP_H_
#define APP_H_

#include <switch.h>

#include "core.h"
#include "user.h"
#include "timer.h"
#include "input.h"
#include "gpu.h"
#include "cmd_list.h"
#include "gfx_pipeline.h"
#include "buffer.h"
#include "texture.h"
#include "bitmap.h"
#include "camera.h"
#include "sampler.h"
#include "descriptor_set.h"
#include "model.h"
#include "heap.h"
#include "audio.h"

typedef struct app_config_t {
    b8 print_to_fb;
    b8 load_user_icon;
    b8 redirect_stdio;
} app_config_t;

typedef struct app_t {
    // @note(ame): Config
    app_config_t *config;

    // @note(ame): docked? handheld?
    AppletOperationMode applet_mode;

    // @note(ame): User
    user_t curr_user;
    pad_t curr_pad;

    // @note(ame): Engine
    i32 nxlink_sock;
    gpu_t gpu;
    stimer_t dt_timer;
    f32 last;

    // @note(kripesh): Audio
    audio_t audio;
    audio_track_t tracks[2];
    
    // @note(ame): renderer
    gfx_pipeline_t tri_pipeline;
    model_t model;
    buffer_t color_buffer[DEFAULT_GPU_FB_COUNT];
    sampler_t my_sampler;
    descriptor_set_t image_set;
    descriptor_set_t sampler_set;

    camera_t camera;
    texture_t depth_target;
    heap_t resize_heap;
} app_t;

void app_init(app_t *app, app_config_t *config);
void app_run(app_t *app);
void app_exit(app_t *app);

#endif
