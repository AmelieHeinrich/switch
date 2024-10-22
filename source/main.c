//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 16:16:57
//

#include "ext_compilation/ext_unit.c"

#include "app.h"
#include "util.h"

int main(int argc, char* argv[])
{
    app_config_t config = {
        .print_to_fb = false,
        .load_user_icon = false,
        .redirect_stdio = true
    };

    app_t app;
    app_init(&app, &config);

    app_run(&app);
    
    app_exit(&app);
    return 0;
}
