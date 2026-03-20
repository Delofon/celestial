#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>

#include "ts.h"
#include "simulation.h"
#include "app.h"
#include "platform.h"

// delta time is always dt
// if computation time fdt turns out to be faster,
// sleep for the time remaining
void fixed_loop(state_t *state, double dt, bool sleep)
{
    struct timespec diff, ts1, ts2;

    CLOCK(ts1);
    ts2 = ts1;
    while(state->loop)
    {
        tsdiff(&diff, &ts2, &ts1);
        scalar_t fdt = tstosdbl(&diff);

        CLOCK(ts1);

        step(state, dt, fdt);

        CLOCK(ts2);

        if(sleep && dt > fdt)
        {
            double waits = dt - fdt;
            double waitus = waits * 1000000.0;
            us_sleep((uint64_t)waitus);
        }

        // so that app thread can have a chance at locking the mutex
        sched_yield();
    }
}
// clamp dt to fdt
// aim for fdt to be tdt
void rt_loop(state_t *state, double tdt)
{
#ifdef VERLET
    fprintf(stderr, "error: Velocity verlet is not symplectic with "
            "variable timestep.\n");
    state->loop = false;
    return; // maybe return value as return code?
#endif

    struct timespec diff, ts1, ts2;

    CLOCK(ts1);
    ts2 = ts1;
    while(state->loop)
    {
        tsdiff(&diff, &ts2, &ts1);
        scalar_t fdt = tstosdbl(&diff);

        CLOCK(ts1);

        step(state, fdt, fdt);

        CLOCK(ts2);

        if(tdt > fdt)
        {
            double waits = tdt - fdt;
            double waitus = waits * 1000000.0;
            us_sleep((uint64_t)waitus);
        }

        sched_yield();
    }
}

void *sim_start(state_t *state)
{
    rt_loop(state, 1.0/240.0);
    //fixed_loop(state, 1.0/240.0, true);

    return 0;
}
int main()
{
    int ret = sdl_init();
    if(ret) return ret;

    state_t *state = malloc(sizeof(state_t));
    state_init(state);

    pthread_t simthr;
    ret = pthread_create(
            &simthr,
            0,
            (void *(*)(void *))sim_start,
            (void *)state
            );
    if(ret != 0)
    {
        fprintf(stderr, "error: could not create simulation thread\n");
        return 1;
    }

    const float fdt = 1.0/240.0;

    while(state->loop)
    {
        draw(state, fdt);
        us_sleep(fdt*1000000);
    }

    return 0;
}

