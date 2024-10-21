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

    app->vert = shader_loader_read(&app->gpu.shader_loader, "romfs:/shaders/tri_vsh.dksh");
    app->frag = shader_loader_read(&app->gpu.shader_loader, "romfs:/shaders/tri_fsh.dksh");
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
                DkViewport viewport = { 0.0f, 0.0f, (float)app->gpu.width, (float)app->gpu.height, 0.0f, 1.0f };
                DkScissor scissor = { 0, 0, app->gpu.width, app->gpu.height };
                DkShader const* shaders[] = { &app->vert, &app->frag };
                DkRasterizerState rasterizerState;
                DkColorState colorState;
                DkColorWriteState colorWriteState;

                // Initialize state structs with the deko3d defaults
                dkRasterizerStateDefaults(&rasterizerState);
                dkColorStateDefaults(&colorState);
                dkColorWriteStateDefaults(&colorWriteState);

                dkCmdBufSetViewports(frame.cmd_buf, 0, &viewport, 1);
                dkCmdBufSetScissors(frame.cmd_buf, 0, &scissor, 1);
                dkCmdBufBindRenderTarget(frame.cmd_buf, &frame.backbuffer_view, NULL);
                if (app->applet_mode == AppletOperationMode_Console) {
                    dkCmdBufClearColorFloat(frame.cmd_buf, 0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
                } else {
                    dkCmdBufClearColorFloat(frame.cmd_buf, 0, DkColorMask_RGBA, 1.0f, 1.0f, 1.0f, 1.0f);
                }
                dkCmdBufBindShaders(frame.cmd_buf, DkStageFlag_GraphicsMask, shaders, 2);
                dkCmdBufBindRasterizerState(frame.cmd_buf, &rasterizerState);
                dkCmdBufBindColorState(frame.cmd_buf, &colorState);
                dkCmdBufBindColorWriteState(frame.cmd_buf, &colorWriteState);
                dkCmdBufDraw(frame.cmd_buf, DkPrimitive_Triangles, 3, 1, 0, 0);
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
