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
#include "input.h"
#include "arena.h"
#include "gpu.h"

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
    DkShader vert;
    DkShader frag;
} app_t;

void app_init(app_t *app, app_config_t *config);
void app_run(app_t *app);
void app_exit(app_t *app);

#endif
