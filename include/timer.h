//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 19:02:37
//

#ifndef TIMER_H_
#define TIMER_H_

#include <switch.h>

#include "core.h"

typedef struct stimer_t {
    u64 start;
} stimer_t;

void timer_init(stimer_t *timer);
void timer_restart(stimer_t *timer);
f32 timer_elapsed(stimer_t *timer);

#endif
