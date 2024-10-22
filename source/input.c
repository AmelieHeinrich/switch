//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:54:14
//

#include <string.h>

#include "input.h"

void input_init(u32 pad_count)
{
    padConfigureInput(pad_count, HidNpadStyleSet_NpadStandard);
}

void pad_init(pad_t *pad)
{
    padInitializeDefault(&pad->pad);
}

void pad_update(pad_t *pad)
{
    padUpdate(&pad->pad);

    pad->kdown = padGetButtonsDown(&pad->pad);
    pad->kheld = padGetButtons(&pad->pad);
    pad->kup = padGetButtons(&pad->pad);

    pad->lstick = padGetStickPos(&pad->pad, 0);
    pad->rstick = padGetStickPos(&pad->pad, 1);

    // @note(ame): apply deadzones
    if (pad->lstick.x > 0 && pad->lstick.x <= 300)
        pad->lstick.x = 0;
    if (pad->lstick.y > 0 && pad->lstick.y <= 300)
        pad->lstick.y = 0;
    if (pad->rstick.x > 0 && pad->rstick.x <= 300)
        pad->rstick.x = 0;
    if (pad->rstick.y > 0 && pad->rstick.y <= 300)
        pad->rstick.y = 0;
}

b8 pad_down(pad_t *pad, u64 bid)
{
    return ((pad->kdown & bid) == 0) ? false : true;
}

b8 pad_held(pad_t *pad, u64 bid)
{
    return ((pad->kheld & bid) == 0) ? false : true;
}

b8 pad_up(pad_t *pad, u64 bid)
{
    return ((pad->kup & bid) == 0) ? false : true;
}

void pad_get_lstick(pad_t *pad, i32 *x, i32 *y)
{
    *x = pad->lstick.x;
    *y = pad->lstick.y;
}

void pad_get_rstick(pad_t *pad, i32 *x, i32 *y)
{
    *x = pad->rstick.x;
    *y = pad->rstick.y;
}
