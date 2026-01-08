#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "simulation.h"

void state_init(state_t *state)
{
#if 0
    body_t bodies[] = {
        { {0,0},  {0,0},  v2_z, 100, 1 } ,
        { {10,0}, {0,10}, v2_z, 1,  1 } ,
    };
#else
#define LSIDE 50
#define SZ (LSIDE*LSIDE)
    body_t bodies[SZ];

    for(int i = 0; i < SZ; i++)
    {
        vec2 p = {i%LSIDE,i/LSIDE};

        scalar_t m = (scalar_t)rand() / RAND_MAX;
        m *= m;

        bodies[i] = (body_t){
            p,
            v2_z,
            v2_z,
            m,
            1
        };
    }
#endif

    state->sz = sizeof(bodies) / sizeof(*bodies);
    state->bodies = malloc(sizeof(bodies));
    memcpy(state->bodies, bodies, sizeof(bodies));

    state->nstep = 0;
    state->loop = true;

    pthread_mutex_init(&state->mtx, 0);
}

void step(state_t *state, scalar_t dt, scalar_t fdt)
{
    body_t *bodies = state->bodies;
    size_t sz = state->sz;

    for(int i = 0; i < sz-1; i++)
    {
        for(int j = i+1; j < sz; j++)
        {
            body_t *a = &bodies[i];
            body_t *b = &bodies[j];

            vec2 ra = b->pos;
            vec2_subeq(&ra, &a->pos);
            vec2 rb = ra;

            scalar_t r = vec2_mag(&ra);
            scalar_t r2 = r*r;
            vec2_vsmuleq(&ra, 1.0/r);

            //scalar_t r2 = vec2_magsqr(&ra);

            scalar_t Wa = G*b->m/r2;
            scalar_t Wb = G*a->m/r2;

            vec2_addeq(&a->acc, vec2_vsmuleq(&ra,  Wa));
            vec2_addeq(&b->acc, vec2_vsmuleq(&rb, -Wb));
        }
    }

    // TODO: Maybe separate arrays for PVWM?
    // W is only relevant to the simulation,
    // whereas P and V are also pulled by the renderer.

    // W doesn't matter for the renderer but PV do
    pthread_mutex_lock(&state->mtx);
    for(int i = 0; i < sz; i++)
    {
        body_t *body = &bodies[i];

        vec2_addeq(&body->vel, vec2_vsmuleq(&body->acc, dt));
        vec2 vdt = body->vel; // preserve velocity
        vec2_addeq(&body->pos, vec2_vsmuleq(&vdt, dt));

        body->acc = v2_z;
    }

#if 1
    printf("step %zu\n", state->nstep);
    printf(" dt %f\n", dt);
    printf("fdt %f\n", fdt);
    printf("ups %.0f\n", 1.0/fdt);
#if 0
    for(int i = 0; i < sz; i++)
    {
        body_t *body = &bodies[i];
        printf(
                "%d: { {%.2f, %.2f}, {%.02f, %.02f}, %.2f }\n",
                i,
                body->pos.x, body->pos.y,
                body->vel.x, body->vel.y,
                body->m
              );
    }
#endif
#endif

    state->nstep++;

    pthread_mutex_unlock(&state->mtx);
}

