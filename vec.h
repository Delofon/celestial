#ifndef VEC_H_
#define VEC_H_

#include <math.h>

#include "def.h"

typedef struct
{
    scalar_t x;
    scalar_t y;
} vec2;

//extern const vec2 v2_z;
#define v2_z ((vec2){ 0.0, 0.0 })

// for some reason, debug builds with G*m/r2 acc
// (as opposed to G*m/r) are slower with static inline
// than without...
#ifndef SI_VEC
vec2 *vec2_addeq(vec2 *restrict a, const vec2 *restrict b);
vec2 *vec2_subeq(vec2 *restrict a, const vec2 *restrict b);
vec2 *vec2_vsmuleq(vec2 *a, const scalar_t b);
vec2 *vec2_vvmuleq(vec2 *restrict a, const vec2 *restrict b);

scalar_t vec2_magsqr(const vec2 *vec);
scalar_t vec2_mag(const vec2 *vec);
#endif

#ifdef SI_VEC
#define SIVEC static inline
#else
#define SIVEC
#endif

#if defined(SI_VEC) || defined(VEC_IMPLEMENTATION)

SIVEC vec2 *vec2_addeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x += b->x;
    a->y += b->y;

    return a;
}
SIVEC vec2 *vec2_subeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x -= b->x;
    a->y -= b->y;

    return a;
}
SIVEC vec2 *vec2_vsmuleq(vec2 *a, const scalar_t b)
{
    a->x *= b;
    a->y *= b;

    return a;
}
SIVEC vec2 *vec2_vvmuleq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x *= b->x;
    a->y *= b->y;

    return a;
}

SIVEC scalar_t vec2_magsqr(const vec2 *vec)
{
    return vec->x*vec->x + vec->y*vec->y;
}
SIVEC scalar_t vec2_mag(const vec2 *vec)
{
    return sqrt(vec->x*vec->x + vec->y*vec->y);
}

#endif // if defined

#endif

