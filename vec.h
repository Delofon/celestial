#ifndef VEC_H_
#define VEC_H_

#ifdef SI_VEC
#include <math.h>
#endif

#include "def.h"

typedef struct
{
    scalar_t x;
    scalar_t y;
} vec2;

extern const vec2 v2_z;

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

#else

static inline vec2 *vec2_addeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x += b->x;
    a->y += b->y;

    return a;
}
static inline vec2 *vec2_subeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x -= b->x;
    a->y -= b->y;

    return a;
}
static inline vec2 *vec2_vsmuleq(vec2 *a, const scalar_t b)
{
    a->x *= b;
    a->y *= b;

    return a;
}
static inline vec2 *vec2_vvmuleq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x *= b->x;
    a->y *= b->y;

    return a;
}

static inline scalar_t vec2_magsqr(const vec2 *vec)
{
    return vec->x*vec->x + vec->y*vec->y;
}
static inline scalar_t vec2_mag(const vec2 *vec)
{
    return sqrt(vec->x*vec->x + vec->y*vec->y);
}

#endif // SI_VEC

#endif

