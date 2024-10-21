//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:04:46
//

#include "app.h"

#include <stdio.h>
#include <unistd.h>

void app_init(app_t *app, app_config_t *config)
{
    app->config = config;
    app->applet_mode = appletGetOperationMode();

    if (config->redirect_stdio) {
        socketInitializeDefault();
        app->nxlink_sock = nxlinkStdioForDebug();
    }

    // @note(ame): init fs
    romfsInit();

    if (config->print_to_fb)
        consoleInit(NULL);

    gpu_config_t gpu_config = {
        .mode = app->applet_mode
    };

    user_collect_data(&app->curr_user, config->load_user_icon);
    pad_init(&app->curr_pad);
    gpu_init(&app->gpu, &gpu_config);

    gfx_pipeline_init(&app->tri_pipeline, &app->gpu, "romfs:/shaders/tri_vsh.dksh", "romfs:/shaders/tri_fsh.dksh");
}

void app_run(app_t *app)
{
    while (appletMainLoop()) {
        // @note(ame): Clear console fb so it updates every frame
        if (app->config->print_to_fb)
            consoleClear();

        // @note(ame): Resize
        AppletOperationMode mode = appletGetOperationMode();
        if (mode != app->applet_mode) {
            gpu_resize(&app->gpu, mode);
            app->applet_mode = mode;
        }

        // @note(ame): Update pad
        pad_update(&app->curr_pad);
        if (pad_down(&app->curr_pad, HidNpadButton_Plus))
            break;

        // @note(ame): MAIN RENDER LOOP
        if (!app->config->print_to_fb) {
            frame_t frame = gpu_begin(&app->gpu);
            {
                cmd_list_viewport_scissor(frame.cmd_buf, (float)app->gpu.width, (float)app->gpu.height);
                dkCmdBufBindRenderTarget(frame.cmd_buf, &frame.backbuffer_view, NULL);
                cmd_list_clear_color(frame.cmd_buf, app->applet_mode == AppletOperationMode_Console ? HMM_V3(0.0f, 0.0f, 0.0f) : HMM_V3(1.0f, 1.0f, 1.0f), 0);
                cmd_list_bind_gfx_pipeline(frame.cmd_buf, &app->tri_pipeline);
                cmd_list_draw(frame.cmd_buf, DkPrimitive_Triangles, 3);
            }
            gpu_end(&app->gpu, &frame);
            gpu_present(&app->gpu);
        }

        // @note(ame): Push console fb to next frame
        if (app->config->print_to_fb)
            consoleUpdate(NULL);
    }
}

void app_exit(app_t *app)
{
    gpu_exit(&app->gpu);
    if (app->config->print_to_fb)
        consoleExit(NULL);
    romfsExit();
    if (app->config->redirect_stdio) {
        close(app->nxlink_sock);
        socketExit();
    }
}
