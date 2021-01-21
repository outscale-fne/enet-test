#include <stdlib.h>
#include "deltatime.h"
#include <stddef.h>
#include <stdio.h>
#include "../timer_lib/timer.h"

tick_t start, time;
tick_t tick, freq, res;
tick_t t1, t2;

void delta_time_init()
{
    timer_lib_initialize();

    res = 0xFFFFFFFFFFFFFFFFULL;
    freq = timer_ticks_per_second();
    start = timer_current();
    t1 = timer_current();
    t2 = timer_current();
}
long double delta_time()
{
    // return ((long double)(t2.tv_nsec - t1.tv_nsec)) / 1000 / 1000;
    return (double)timer_ticks_to_seconds(t2 - t1);
}
void delta_time_update()
{
    //printf("delta: %Lf\n", delta_time());
    t1 = t2;
    t2 = timer_current();
}
void delta_time_exit()
{
    timer_lib_shutdown();
}