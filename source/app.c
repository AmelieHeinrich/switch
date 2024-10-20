//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:04:46
//

#include "app.h"

#include <stdio.h>

void app_init(app_t *app, app_config_t *config)
{
    app->config = config;

    if (config->print_to_fb)
        consoleInit(NULL);

    user_collect_data(&app->curr_user, config->load_user_icon);
    pad_init(&app->curr_pad);
}

void app_run(app_t *app)
{
    while (appletMainLoop()) {
        // Clear console fb so it updates every frame
        if (app->config->print_to_fb)
            consoleClear();
        printf("Current user: %s", app->curr_user.nickname);

        pad_update(&app->curr_pad);
        if (pad_down(&app->curr_pad, HidNpadButton_Plus))
            break;

        // Push console fb to next frame
        if (app->config->print_to_fb)
            consoleUpdate(NULL);
    }
}

void app_exit(app_t *app)
{
    if (app->config->print_to_fb)
        consoleExit(NULL);
}
