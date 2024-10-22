//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 19:27:22
//

#include "timer.h"

void timer_init(stimer_t *timer)
{
    timer->start = armGetSystemTick();
}

void timer_restart(stimer_t *timer)
{
    timer->start = armGetSystemTick();
}

f32 timer_elapsed(stimer_t *timer)
{
    u64 ns = armGetSystemTick();
    return (ns - timer->start) / 1000000000.0f;
}
