//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 17:45:10
//

#ifndef INPUT_H_
#define INPUT_H_

#include <switch.h>

#include "core.h"

// @note(ame): pad id doesn't work yet, reads input from first controller instead

typedef struct pad_t {
    PadState pad;
    u32 pad_id;

    u64 kdown;
    u64 kheld;
    u64 kup;

    HidAnalogStickState lstick;
    HidAnalogStickState rstick;
} pad_t;

void input_init(u32 pad_count);

void pad_init(pad_t *pad);
void pad_update(pad_t *pad);
b8 pad_down(pad_t *pad, u64 bid);
b8 pad_held(pad_t *pad, u64 bid);
b8 pad_up(pad_t *pad, u64 bid);
void pad_get_lstick(pad_t *pad, i32 *x, i32 *y);
void pad_get_rstick(pad_t *pad, i32 *x, i32 *y);

#endif
