//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:04:46
//

#include "app.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

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
    input_init(1);
    pad_init(&app->curr_pad);
    gpu_init(&app->gpu, &gpu_config);

    app->tri_pipeline.states[0] = (DkVtxAttribState){ 0, 0, 0, DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 };
    app->tri_pipeline.states[1] = (DkVtxAttribState){ 0, 0, sizeof(float) * 3, DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 };
    app->tri_pipeline.states[2] = (DkVtxAttribState){ 0, 0, sizeof(float) * 5, DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 };
    app->tri_pipeline.state_count = 3;
    gfx_pipeline_init(&app->tri_pipeline, &app->gpu, "romfs:/shaders/tri_vsh.dksh", "romfs:/shaders/tri_fsh.dksh");

    model_load(&app->model, &app->gpu, "romfs:/assets/models/Cube.gltf");

    for (i32 i = 0; i < DEFAULT_GPU_FB_COUNT; i++) {
        buffer_init(&app->color_buffer[i], &app->gpu.uniform_heap, 256, 0);
    }

    timer_init(&app->dt_timer);
    camera_init(&app->camera);
    camera_resize(&app->camera, app->applet_mode == AppletOperationMode_Console ? DOCKED_WIDTH : UNDOCKED_WIDTH,
                                app->applet_mode == AppletOperationMode_Console ? DOCKED_HEIGHT : UNDOCKED_HEIGHT);
    
    texture_init(&app->depth_target, app->gpu.width, app->gpu.height, DkImageFormat_ZF32, &app->gpu.image_heap, app->gpu.device, DkImageFlags_UsageRender | DkImageFlags_HwCompression);

    // @note(ame): texture creation
    bitmap_t bitmap = bitmap_load("romfs:/assets/textures/texture.jpg");
    texture_init(&app->my_texture, bitmap.width, bitmap.height, DkImageFormat_RGBA8_Unorm, &app->gpu.image_heap, app->gpu.device, 0);

    buffer_t staging;
    buffer_init(&staging, &app->gpu.data_heap, app->my_texture.size, 0);
    buffer_upload(&staging, bitmap.pixels, app->my_texture.size);

    cmd_list_single_use_t uploader = cmd_list_begin_single_use(app->gpu.device, &app->gpu.cmd_heap);
    cmd_list_copy_buffer_to_texture(uploader.buf, &staging, &app->my_texture);
    cmd_list_end_single_use(&uploader, app->gpu.queue);

    gpu_wait(&app->gpu);

    // @note(ame): dset and sampler
    app->my_sampler = sampler_init(DkFilter_Linear, DkWrapMode_Repeat);
    app->image_set = descriptor_set_init(&app->gpu.descriptor_heap, 1);
    app->sampler_set = descriptor_set_init(&app->gpu.descriptor_heap, 1);

    // @note(ame): cleanup
    cmd_list_free_single_use(&uploader);
    buffer_free(&staging);
    bitmap_free(&bitmap);
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
            i32 w = mode == AppletOperationMode_Console ? DOCKED_WIDTH : UNDOCKED_WIDTH;
            i32 h = mode == AppletOperationMode_Console ? DOCKED_HEIGHT : UNDOCKED_HEIGHT;

            gpu_resize(&app->gpu, mode);

            texture_free(&app->depth_target);
            texture_init(&app->depth_target, app->gpu.width, app->gpu.height, DkImageFormat_ZF32, &app->gpu.image_heap, app->gpu.device, DkImageFlags_UsageRender | DkImageFlags_HwCompression);      
            
            camera_resize(&app->camera, w, h);
            app->applet_mode = mode;
        }

        // @note(ame): Update pad
        pad_update(&app->curr_pad);
        if (pad_down(&app->curr_pad, HidNpadButton_Plus))
            break;

        // @note(ame): Update camera
        camera_update(&app->camera);
        camera_input(&app->camera, &app->curr_pad);

        // @note(ame): MAIN RENDER LOOP
        if (!app->config->print_to_fb) {
            frame_t frame = gpu_begin(&app->gpu);
            {
                HMM_Mat4 matrices[] = { app->camera.projection, app->camera.view };
                buffer_upload(&app->color_buffer[frame.frame_idx], matrices, sizeof(matrices));

                texture_t *targets[] = {
                    frame.backbuffer
                };

                cmd_list_viewport_scissor(frame.cmd_buf, (float)app->gpu.width, (float)app->gpu.height);
                cmd_list_bind_render_targets(frame.cmd_buf, targets, 1, &app->depth_target);
                cmd_list_clear_color(frame.cmd_buf, app->applet_mode == AppletOperationMode_Console ? HMM_V3(0.0f, 0.0f, 0.0f) : HMM_V3(1.0f, 1.0f, 1.0f), 0);
                cmd_list_clear_depth(frame.cmd_buf);
                cmd_list_bind_gfx_pipeline(frame.cmd_buf, &app->tri_pipeline);
                cmd_list_dset_write_texture(frame.cmd_buf, &app->image_set, &app->my_texture, 0);
                cmd_list_dset_write_sampler(frame.cmd_buf, &app->sampler_set, &app->my_sampler, 0);
                cmd_list_bind_image_dset(frame.cmd_buf, &app->image_set);
                cmd_list_bind_sampler_dset(frame.cmd_buf, &app->sampler_set);
                cmd_list_bind_texture(frame.cmd_buf, DkStage_Fragment, 0, 0);
                cmd_list_bind_uni_buffer(frame.cmd_buf, &app->color_buffer[frame.frame_idx], 0, DkStage_Vertex);
                for (int i = 0; i < app->model.submesh_count; i++) {
                    cmd_list_bind_vtx_buffer(frame.cmd_buf, &app->model.submeshes[i].vertex_buffer);
                    cmd_list_bind_idx_buffer(frame.cmd_buf, &app->model.submeshes[i].index_buffer);
                    cmd_list_draw_indexed(frame.cmd_buf, DkPrimitive_Triangles, app->model.submeshes[i].index_count);
                }
            }
            gpu_end(&app->gpu, &frame);
            gpu_present(&app->gpu);
        }
    }
}

void app_exit(app_t *app)
{
    gpu_wait(&app->gpu);

    texture_free(&app->depth_target);
    texture_free(&app->my_texture);
    for (i32 i = 0; i < DEFAULT_GPU_FB_COUNT; i++) {
        buffer_free(&app->color_buffer[i]);
    }
    model_free(&app->model);
    gpu_exit(&app->gpu);
    if (app->config->print_to_fb)
        consoleExit(NULL);
    romfsExit();
    if (app->config->redirect_stdio) {
        close(app->nxlink_sock);
        socketExit();
    }
}
