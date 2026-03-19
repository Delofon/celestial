#ifndef VEC_H_
#define VEC_H_

#include <math.h>

#include "def.h"

typedef struct
{
    scalar_t x;
    scalar_t y;
} vec2;

#define V2L(x, y) ((vec2){ (x), (y) })
#define v2_z V2L(0.0, 0.0)

// for some reason, debug builds with G*m/r2 acc
// (as opposed to G*m/r) are slower with static inline
// than without...
#ifndef SI_VEC

// v2_ operate on structs, therefore likely
// implying copying without SI or -flto
// v2p_ operate on pointers, therefore 100% avoiding
// copying but possibly introducing overhead without SI or -flto
// (I also reckon it's better to pass scalars as is regardless)
// v2_*eq is a mix: first arg is vec2*,
// second arg and return is just vec2

vec2 v2_add(const vec2 a, const vec2 b);
vec2 v2_sub(const vec2 a, const vec2 b);
vec2 v2_vsmul(const vec2 a, const scalar_t b);
vec2 v2_vvmul(const vec2 a, const vec2 b);
vec2 v2_addeq(vec2 *a, const vec2 b);
vec2 v2_subeq(vec2 *a, const vec2 b);
vec2 v2_vsmuleq(vec2 *a, const scalar_t b);
vec2 v2_vvmuleq(vec2 *a, const vec2 b);

scalar_t v2_magsqr(const vec2 vec);
scalar_t v2_mag(const vec2 vec);

vec2 *v2p_addeq(vec2 *restrict a, const vec2 *restrict b);
vec2 *v2p_subeq(vec2 *restrict a, const vec2 *restrict b);
vec2 *v2p_vsmuleq(vec2 *a, const scalar_t b);
vec2 *v2p_vvmuleq(vec2 *restrict a, const vec2 *restrict b);

scalar_t v2p_magsqr(const vec2 *vec);
scalar_t v2p_mag(const vec2 *vec);
#endif

#ifdef SI_VEC
#define SIVEC static inline
#else
#define SIVEC
#endif

#if defined(SI_VEC) || defined(VEC_IMPLEMENTATION)

SIVEC vec2 v2_add(const vec2 a, const vec2 b)
{
    return V2L(a.x + b.x, a.y + b.y);
}
SIVEC vec2 v2_sub(const vec2 a, const vec2 b)
{
    return V2L(a.x - b.x, a.y - b.y);
}
SIVEC vec2 v2_vsmul(const vec2 a, const scalar_t b)
{
    return V2L(a.x * b, a.y * b);
}
SIVEC vec2 v2_vvmul(const vec2 a, const vec2 b)
{
    return V2L(a.x * b.x, a.y * b.y);
}
SIVEC vec2 v2_addeq(vec2 *a, const vec2 b)
{
    a->x += b.x;
    a->y += b.y;
    return *a;
}
SIVEC vec2 v2_subeq(vec2 *a, const vec2 b)
{
    a->x -= b.x;
    a->y -= b.y;
    return *a;
}
SIVEC vec2 v2_vsmuleq(vec2 *a, const scalar_t b)
{
    a->x *= b;
    a->y *= b;
    return *a;
}
SIVEC vec2 v2_vvmuleq(vec2 *a, const vec2 b)
{
    a->x *= b.x;
    a->y *= b.y;
    return *a;
}

SIVEC scalar_t v2_magsqr(const vec2 vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}
SIVEC scalar_t v2_mag(const vec2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

SIVEC vec2 *v2p_addeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x += b->x;
    a->y += b->y;

    return a;
}
SIVEC vec2 *v2p_subeq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x -= b->x;
    a->y -= b->y;

    return a;
}
SIVEC vec2 *v2p_vsmuleq(vec2 *a, const scalar_t b)
{
    a->x *= b;
    a->y *= b;

    return a;
}
SIVEC vec2 *v2p_vvmuleq(vec2 *restrict a, const vec2 *restrict b)
{
    a->x *= b->x;
    a->y *= b->y;

    return a;
}

SIVEC scalar_t v2p_magsqr(const vec2 *vec)
{
    return vec->x*vec->x + vec->y*vec->y;
}
SIVEC scalar_t v2p_mag(const vec2 *vec)
{
    return sqrt(vec->x*vec->x + vec->y*vec->y);
}

#endif // if defined

#endif

