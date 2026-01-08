#ifndef SIM_H_
#define SIM_H_

#include <stdbool.h>
#include <stddef.h>

#include <pthread.h>

#include "def.h"
#include "vec.h"

#define G 10.0

typedef struct
{
    vec2 pos;
    vec2 vel;
    vec2 acc; // Move outside?
    scalar_t m;

    bool active;
} body_t;

typedef struct
{
    pthread_mutex_t mtx;

    body_t *bodies;
    size_t sz;

    size_t nstep;
    bool loop;
} state_t;

void state_init(state_t *state);
void step(state_t *state, scalar_t dt, scalar_t fdt);

#endif

