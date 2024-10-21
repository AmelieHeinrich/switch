//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 20:24:01
//

#include "util.h"

void throw_error(const char* title, const char* message, int error_code)
{
    ErrorApplicationConfig c;
    errorApplicationCreate(&c, title, message);
    errorApplicationSetNumber(&c, error_code);
    errorApplicationShow(&c);
    __builtin_trap();
}
