#include <math.h>

#include "vec.h"

const vec2 v2_z = {0.0,0.0};

// see vec.h:18
#ifndef SI_VEC

vec2 *vec2_addeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x += b->x;
    a->y += b->y;

    return a;
}
vec2 *vec2_subeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x -= b->x;
    a->y -= b->y;

    return a;
}
vec2 *vec2_vsmuleq(vec2 *a, const scalar_t b)
{
    a->x *= b;
    a->y *= b;

    return a;
}
vec2 *vec2_vvmuleq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x *= b->x;
    a->y *= b->y;

    return a;
}

scalar_t vec2_magsqr(const vec2 *vec)
{
    return vec->x*vec->x + vec->y*vec->y;
}
scalar_t vec2_mag(const vec2 *vec)
{
    return sqrt(vec->x*vec->x + vec->y*vec->y);
}

#endif // SI_VEC

