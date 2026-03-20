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
        { {0,  0}, {0, 0},   100 },
        { {10, 0}, {0, 1},   1   },
        { {40, 0}, {0, 0.3}, 1 }
    };
#else
#define LSIDE 30
#define SZ (LSIDE*LSIDE)
    body_t bodies[SZ];

    for(int i = 0; i < SZ; i++)
    {
        vec2 p = {i%LSIDE * 30.0, (int)(i/LSIDE) * 30.0};

        scalar_t m = (scalar_t)rand() / RAND_MAX;
        m = powf(m, 10.0);
        m *= 10.0;

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
#ifdef VERLET
    state->acc_dt = calloc(state->sz, sizeof(*state->acc_dt));
#endif
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

void calculate_acc(const state_t *state, vec2 *acc)
{
    idx_t *bodies = state->bodies;
    size_t sz = state->sz;

    for(int i = 0; i < sz-1; i++)
    {
        if(!bodies[i].active || !bodies[i].not_removed)
            continue;

        for(int j = i+1; j < sz; j++)
        {
            if(!bodies[j].active || !bodies[j].not_removed)
                continue;

            idx_t *a = &bodies[i];
            idx_t *b = &bodies[j];

            scalar_t ma = state->m[a->idx];
            scalar_t mb = state->m[b->idx];

            vec2 ra = state->pos[b->idx];
            v2p_subeq(&ra, &state->pos[a->idx]);

            scalar_t r2 = v2_magsqr(ra);
            scalar_t r = sqrtf(r2);

            v2_vsmuleq(&ra, 1.0/r);

            vec2 rb = ra;
            scalar_t Wa;
            scalar_t Wb;

            if(r2 > RADIUS_LINEAR)
            {
                Wa = G*mb/r2;
                Wb = G*ma/r2;
            }
            else
            {
                // use a linear law if the bodies are too close
                // as per the shell theorem

                Wa = G*mb*r;
                Wb = G*ma*r;
            }

            v2p_addeq(&acc[a->idx], v2p_vsmuleq(&ra,  Wa));
            v2p_addeq(&acc[b->idx], v2p_vsmuleq(&rb, -Wb));
        }
    }
}

void step(state_t *state, scalar_t dt, scalar_t fdt)
{
    idx_t *bodies = state->bodies;
    size_t sz = state->sz;

#ifndef VERLET
    // use semi-implicit/symplectic Euler integrator
    calculate_acc(state, state->acc);

    // W doesn't matter for the renderer but PV do
    pthread_mutex_lock(&state->mtx);
    for(int i = 0; i < sz; i++)
    {
        uint64_t idx = bodies[i].idx;

        v2p_addeq(&state->vel[idx], v2p_vsmuleq(&state->acc[idx], dt));
        vec2 vdt = state->vel[idx]; // preserve velocity
        v2p_addeq(&state->pos[idx], v2p_vsmuleq(&vdt, dt));

        state->acc[idx] = v2_z;
    }
#else
    // use Verlet integrator

    pthread_mutex_lock(&state->mtx);
    for(int i = 0; i < sz; i++)
    {
        idx_t *body = &bodies[i];
        uint64_t idx = body->idx;

        v2p_vsmuleq(&state->acc[idx], dt*0.5);

        vec2 v = state->vel[idx];
        v2p_vsmuleq(&v, dt);

        v2p_addeq(&state->vel[idx], &state->acc[idx]);

        v2p_vsmuleq(&state->acc[idx], dt);

        v2p_addeq(&state->pos[idx], &v);
        v2p_addeq(&state->pos[idx], &state->acc[idx]);
    }
    pthread_mutex_unlock(&state->mtx);

    calculate_acc(state, state->acc_dt);

    pthread_mutex_lock(&state->mtx);
    for(int i = 0; i < sz; i++)
    {
        idx_t *body = &bodies[i];
        uint64_t idx = body->idx;

        state->acc[idx] = state->acc_dt[idx];
        v2p_vsmuleq(&state->acc_dt[idx], dt*dt*0.5);
        v2p_addeq(&state->vel[idx], &state->acc_dt[idx]);

        state->acc_dt[idx] = v2_z;
    }
#endif

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

