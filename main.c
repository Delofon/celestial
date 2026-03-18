#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#include "ts.h"
#include "simulation.h"
#include "app.h"

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
            usleep((uint64_t)waitus);
        }
    }
}
void *sim_start(state_t *state)
{
    //rt_loop(&state, 1.0/60.0);
    fixed_loop(state, 1.0/240.0, true);

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
    assert(ret == 0 && "could not create simulation thread");

    const float fdt = 1.0/240.0;

#if 1
    while(state->loop)
    {
        draw(state, fdt);
        usleep(fdt*1000000);
    }
#else
    pthread_join(simthr, 0);
#endif

    return 0;
}

