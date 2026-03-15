#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <pthread.h>

#include "simulation.h"

void state_init(state_t *state)
{
#if 0
    body_t bodies[] = {
        { {0,0},  {0,0},  100 },
        { {10,0}, {0,10}, 1   }
    };
#else
#define LSIDE 16
#define SZ (LSIDE*LSIDE)
    body_t bodies[SZ];

    for(int i = 0; i < SZ; i++)
    {
        vec2 p = {i%LSIDE,i/LSIDE};

        scalar_t m = (scalar_t)rand() / RAND_MAX * 3;
        m *= m;
        m *= m;

        bodies[i] = (body_t){
            p,
            v2_z,
            m
        };
    }
#endif

    state->sz = sizeof(bodies) / sizeof(*bodies);

    state->bodies = calloc(state->sz, sizeof(*state->bodies));
    state->pos    = calloc(state->sz, sizeof(*state->pos));
    state->vel    = calloc(state->sz, sizeof(*state->vel));
    state->acc    = calloc(state->sz, sizeof(*state->acc));
    state->m      = calloc(state->sz, sizeof(*state->m));

    for(int i = 0; i < state->sz; i++)
    {
        state_pushbody(state, &bodies[i]);
    }

    state->nstep = 0;
    state->loop = true;

    pthread_mutex_init(&state->mtx, 0);
}

void state_pushbody(state_t *state, body_t *body)
{
    // FIXME: look into proper memory allocators
    for(int i = 0; i < state->sz; i++)
    {
        if(!state->bodies[i].not_removed)
        {
            state->bodies[i].idx = i;
            state->bodies[i].active = true;
            state->bodies[i].not_removed = true;
            state->pos[i] = body->pos;
            state->vel[i] = body->vel;
            state->m[i]   = body->m;
            break;
        }
    }
}

void calculate_acc(state_t *state)
{
    idx_t *bodies = state->bodies;
    size_t sz = state->sz;

    for(int i = 0; i < sz-1; i++)
    {
        for(int j = i+1; j < sz; j++)
        {
            idx_t *a = &bodies[i];
            idx_t *b = &bodies[j];

            scalar_t ma = state->m[a->idx];
            scalar_t mb = state->m[b->idx];

            vec2 ra = state->pos[b->idx];
            vec2_subeq(&ra, &state->pos[a->idx]);

            scalar_t r2 = vec2_magsqr(&ra);
            scalar_t r = sqrtf(r2);

            vec2_vsmuleq(&ra, 1.0/r);

            vec2 rb = ra;

            scalar_t Wa = G*mb/r2;
            scalar_t Wb = G*ma/r2;

            vec2_addeq(&state->acc[a->idx], vec2_vsmuleq(&ra,  Wa));
            vec2_addeq(&state->acc[b->idx], vec2_vsmuleq(&rb, -Wb));
        }
    }
}

void step(state_t *state, scalar_t dt, scalar_t fdt)
{
    idx_t *bodies = state->bodies;
    size_t sz = state->sz;

    calculate_acc(state);

    // W doesn't matter for the renderer but PV do
    pthread_mutex_lock(&state->mtx);
    for(int i = 0; i < sz; i++)
    {
        uint64_t idx = bodies[i].idx;

        vec2_addeq(&state->vel[idx], vec2_vsmuleq(&state->acc[idx], dt));
        vec2 vdt = state->vel[idx]; // preserve velocity
        vec2_addeq(&state->pos[idx], vec2_vsmuleq(&vdt, dt));

        state->acc[idx] = v2_z;
    }

#if 1
    printf("step %zu\n", state->nstep);
    printf(" dt %f\n", dt);
    printf("fdt %f\n", fdt);
    printf("ups %.0f\n", 1.0/fdt);
    if(0) {
    for(int i = 0; i < sz; i++)
    {
        idx_t *body = &bodies[i];
        printf(
                "%d: { {%.2f, %.2f}, {%.02f, %.02f}, %.2f }\n",
                i,
                state->pos[body->idx].x, state->pos[body->idx].y,
                state->vel[body->idx].x, state->vel[body->idx].y,
                state->m[body->idx]
              );
    }
    }
#endif

    state->nstep++;
    pthread_mutex_unlock(&state->mtx);
}

