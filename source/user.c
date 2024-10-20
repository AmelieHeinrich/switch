//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:21:24
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "user.h"

b8 user_collect_data(user_t *user, b8 load_icon)
{
    Result rc = 0;
    memset(user, 0, sizeof(user_t));

    // Init
    rc = accountInitialize(AccountServiceType_Application);
    if (R_FAILED(rc)) {
        printf("accountInitialize() failed: 0x%x\n", rc);
        return false;
    }

    // Get the actual user
    rc = accountGetPreselectedUser(&user->user_id);
    if (R_FAILED(rc)) {
        printf("accountGetPreselectedUser() failed: 0x%x, using pselShowUserSelector..\n", rc);

        /* Create player selection UI settings */
        PselUserSelectionSettings settings;
        memset(&settings, 0, sizeof(settings));

        rc = pselShowUserSelector(&user->user_id, &settings);

        if (R_FAILED(rc)) {
            printf("pselShowUserSelector() failed: 0x%x\n", rc);
            return false;
        }
    } else {
        rc = accountGetProfile(&user->profile, user->user_id);
        if (R_FAILED(rc)) {
            printf("accountGetProfile() failed: 0x%x\n", rc);
            return false;
        }
    }

    // Get user data
    rc = accountProfileGet(&user->profile, &user->user_data, &user->profile_base);
    if (R_FAILED(rc)) {
        printf("accountProfileGet() failed: 0x%x\n", rc);
    }

    if (R_SUCCEEDED(rc)) {
        // Copy nickname
        memset(user->nickname,  0, sizeof(user->nickname));
        strncpy(user->nickname, user->profile_base.nickname, sizeof(user->nickname)-1);//Copy the nickname elsewhere to make sure it's NUL-terminated

        // Load icon JPEG
        if (load_icon) {
            rc = accountProfileGetImageSize(&user->profile, &user->jpegSize);
            if (R_FAILED(rc)) {
                printf("accountProfileGetImageSize() failed: 0x%x\n", rc);
                user->valid_icon = false;
            }

            user->jpegBuffer = malloc(user->jpegSize);

            u32 read = 0;
            rc = accountProfileLoadImage(&user->profile, user->jpegBuffer, user->jpegSize, &read);
            if (R_FAILED(rc)) {
                printf("accountProfileLoadImage() failed: 0x%x\n", rc);
                user->valid_icon = false;
            }

            if (read != user->jpegSize) {
                user->valid_icon = false;
            }

            user->valid_icon = true;
        }
    }

    return true;
}

void user_free(user_t *user)
{
    if (user->valid_icon)
        free(user->jpegBuffer);
}
