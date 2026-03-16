#include <stdint.h>
#include <math.h>

#include "color.h"

void rgb_norm(rgb_t *n, const rgb_int_t *restrict rgb)
{
    n->r = rgb->r / 255.0f;
    n->g = rgb->g / 255.0f;
    n->b = rgb->b / 255.0f;
}
void rgb_denorm(rgb_int_t *rgb, const rgb_t *restrict n)
{
    rgb->r = n->r * 255.0f;
    rgb->g = n->g * 255.0f;
    rgb->b = n->b * 255.0f;
}

const float pi_3 = M_PI / 3.0f;

void rgb2hsv(hsv_t *hsv, const rgb_t *restrict rgb)
{
    float min = rgb->r;
    float max = rgb->r;

    if(rgb->g > max)
        max = rgb->g;
    if(rgb->b > max)
        max = rgb->b;
    hsv->v = max;

    if(rgb->g < min)
        min = rgb->g;
    if(rgb->b < min)
        min = rgb->b;

    float c = max - min;

    float h = pi_3;
    if(c == 0)
        h = 0;
    else if(max == rgb->r)
        h *= fmodf(((rgb->g - rgb->b)/c), 6.0f);
    else if(max == rgb->g)
        h *= (rgb->b - rgb->r)/c + 2;
    else if(max == rgb->b)
        h *= (rgb->r - rgb->g)/c + 4;
    else; // unreachable

    h = fmodf(h, 2.0 * M_PI);
    hsv->h = h;

    if(max == 0)
        hsv->s = 0;
    else
        hsv->s = c/max;
}

int abs(int a)
{
    if(a < 0)
        return -a;
    return a;
}

void hsv2rgb(rgb_t *rgb, const hsv_t *restrict hsv)
{
    float c = hsv->v * hsv->s;
    float h1 = hsv->h / pi_3;
    int h1_int = (int)h1;

    float x = c * (1 - fabsf(fmodf(h1, 2.0f) - 1));

    switch(h1_int)
    {
        case 0:
            rgb->r = c;
            rgb->g = x;
            rgb->b = 0;
            break;
        case 1:
            rgb->r = x;
            rgb->g = c;
            rgb->b = 0;
            break;
        case 2:
            rgb->r = 0;
            rgb->g = c;
            rgb->b = x;
            break;
        case 3:
            rgb->r = 0;
            rgb->g = x;
            rgb->b = c;
            break;
        case 4:
            rgb->r = x;
            rgb->g = 0;
            rgb->b = c;
            break;
        case 5:
            rgb->r = c;
            rgb->g = 0;
            rgb->b = x;
            break;
        default:
            // unreachable
            break;
    }

    float m = hsv->v - c;

    rgb->r += m;
    rgb->g += m;
    rgb->b += m;
}

