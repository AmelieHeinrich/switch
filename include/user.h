//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:18:22
//

#ifndef USER_H_
#define USER_H_

#include <switch.h>

#include "core.h"

typedef struct user_t {
    AccountUid user_id;
    AccountProfile profile;
    AccountUserData user_data;
    AccountProfileBase profile_base;

    char nickname[64];

    b8 valid_icon;
    uint32_t jpegSize;
    char *jpegBuffer;
} user_t;

b8 user_collect_data(user_t *user, b8 load_icon);
void user_free(user_t *user);

#endif
