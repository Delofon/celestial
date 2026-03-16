#ifndef COLOR_H_
#define COLOR_H_

#include <stdint.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba_int_t;
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_int_t;
typedef struct
{
    float r;
    float g;
    float b;
} rgb_t;

typedef struct
{
    float h;
    float s;
    float v;
} hsv_t;

void rgb_norm(rgb_t *n, const rgb_int_t *restrict rgb);
void rgb_denorm(rgb_int_t *rgb, const rgb_t *restrict n);

void rgb2hsv(hsv_t *hsv, const rgb_t *restrict rgb);
void hsv2rgb(rgb_t *rgb, const hsv_t *restrict hsv);

#endif

