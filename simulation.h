#ifndef SIM_H_
#define SIM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <pthread.h>

#include "def.h"
#include "vec.h"

#define G 20.0
// max distance between points to apply linear law
#define RADIUS_LINEAR .25

typedef struct
{
    uint64_t idx; // index into pos/vel for body with this id
                  // (id is the index into idx_t*)
    bool active;
    bool not_removed;
} idx_t;

typedef struct
{
    vec2 pos;
    vec2 vel;
    scalar_t m;
} body_t;

typedef struct
{
    pthread_mutex_t mtx;

    idx_t *bodies;
    scalar_t *m;
    vec2 *pos;
    vec2 *vel;
    vec2 *acc;
#ifdef VERLET
    vec2 *acc_dt;
#endif
    size_t sz;

    size_t nstep;
    bool loop;
} state_t;

void state_init(state_t *state);
void state_pushbody(state_t *state, body_t *body);
void step(state_t *state, scalar_t dt, scalar_t fdt);

#endif

