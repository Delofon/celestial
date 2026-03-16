#include <unistd.h>

#include <pthread.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include "color.h"
#include "simulation.h"

SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;

struct {
    vec2 p;
    vec2 s;
    scalar_t z;
} cam;

float camspeed = 250;

int sdl_init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(
                stderr,
                "error: could not init SDL2: %s\n",
                SDL_GetError()
               );
        return 1;
    }

    window = SDL_CreateWindow(
            "celestial",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1280, 720,
            SDL_WINDOW_SHOWN
            );
    if(!window)
    {
        fprintf(
                stderr,
                "error: could not create window: %s\n",
                SDL_GetError()
               );
        return 1;
    }
    renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED
            );
    if(!renderer)
    {
        fprintf(
                stderr,
                "error: could not create renderer: %s\n",
                SDL_GetError()
               );
        return 1;
    }
    surface = SDL_GetWindowSurface(window);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    cam.p = v2_z;
    cam.s = (vec2){w, h};
    cam.z = 10;

    return 0;
}

vec2 wstoss(const vec2 *p)
{
    vec2 v;
    v.x =  (p->x - cam.p.x) * cam.z + cam.s.x/2;
    v.y = -(p->y - cam.p.y) * cam.z + cam.s.y/2;
    return v;
}

bool sdl_kb[SDL_NUM_SCANCODES];
bool sdl_kb_prev[SDL_NUM_SCANCODES];

void process_events(state_t *state)
{
    memcpy(sdl_kb_prev, sdl_kb, sizeof(sdl_kb));

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_QUIT:
                state->loop = false;
                break;
            case SDL_KEYDOWN:
                sdl_kb[e.key.keysym.scancode] = true;
                break;
            case SDL_KEYUP:
                sdl_kb[e.key.keysym.scancode] = false;
                break;
        }
    }
}
bool keyheld(SDL_Scancode sc)
{
    return sdl_kb[sc];
}
bool keydown(SDL_Scancode sc)
{
    return sdl_kb[sc] && !sdl_kb_prev[sc];
}
bool keyup(SDL_Scancode sc)
{
    return !sdl_kb[sc] && sdl_kb_prev[sc];
}

void input(state_t *state, float fdt)
{
    if(keyheld(SDL_SCANCODE_W))
    {
        cam.p.y += camspeed / cam.z * fdt;
    }
    if(keyheld(SDL_SCANCODE_S))
    {
        cam.p.y -= camspeed / cam.z * fdt;
    }
    if(keyheld(SDL_SCANCODE_A))
    {
        cam.p.x -= camspeed / cam.z * fdt;
    }
    if(keyheld(SDL_SCANCODE_D))
    {
        cam.p.x += camspeed / cam.z * fdt;
    }
    if(keydown(SDL_SCANCODE_MINUS))
    {
        cam.z /= 1.5;
    }
    if(keydown(SDL_SCANCODE_EQUALS))
    {
        cam.z *= 1.5;
    }

    if(keyup(SDL_SCANCODE_Q))
    {
        state->loop = false;
    }
}

void draw(state_t *state, float fdt)
{
    process_events(state);
    input(state, fdt);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_FRect rect = {
        .x = 0, .y = 0,
        .w = 5.f, .h = 5.f
    };

    scalar_t m = -1;

    // TODO: we probably don't want to lock the actual
    // online state for the duration of the frame,
    // only copy it in a safe way that should support
    // dynamically changing bodies array
    if(!pthread_mutex_trylock(&state->mtx))
    {
        for(int i = 0; i < state->sz; i++)
        {
            if(state->m[i] > m)
                m = state->m[i];
        }

        for(int i = 0; i < state->sz; i++)
        {
            vec2 *p = &state->pos[i];

            vec2 o = wstoss(p);
            rect.x = o.x; rect.y = o.y;

            float h = state->m[i] / m * 2.0 * M_PI - (M_PI/3);
            h = fmodf(h, 2.0*M_PI);
            rgb_t rgb; hsv_t hsv = (hsv_t){ h, 1, 1 };
            hsv2rgb(&rgb, &hsv);
            rgb_int_t rgb_int;
            rgb_denorm(&rgb_int, &rgb);
            SDL_SetRenderDrawColor(renderer, rgb_int.r, rgb_int.g, rgb_int.b, 100);

            SDL_RenderFillRectF(renderer, &rect);
        }
    }
    pthread_mutex_unlock(&state->mtx);

    SDL_RenderPresent(renderer);
}

