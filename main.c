#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include "ts.h"
#include "simulation.h"

void fixed_loop(state_t *state, double dt, bool sleep)
{
    struct timespec diff, ts1, ts2;

    CLOCK(ts1);
    ts2 = ts1;
    while(state->loop)
    {
        tsdiff(&diff, &ts2, &ts1);
        scalar_t fdt = tstosdbl(&diff);

        CLOCK(ts1);

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    state->loop = false;
                    break;
            }
        }
        step(state, dt, fdt);

        CLOCK(ts2);

        if(sleep && dt > fdt)
        {
            double waits = dt - fdt;
            double waitus = waits * 1000000.0;
            usleep((uint64_t)waitus);
        }
    }
}

void *sim_start(state_t *state)
{
    //rt_loop(&state, 1.0/60.0);
    //fixed_loop(state, 1.0/240.0, true);
    fixed_loop(state, .005, true);

    return 0;
}

SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;

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
    return 0;
}

void wstoss(float *restrict x, float *restrict y,
        const vec2 *restrict p,
        const vec2 *restrict cam_pos,
        const scalar_t scale_factor
        )
{
    *x =  (p->x - cam_pos->x) * scale_factor + 1280/2;
    *y = -(p->y - cam_pos->y) * scale_factor + 720/2;
}

void draw(state_t *state)
{
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    SDL_RenderClear(renderer);

    // TODO: we probably don't want to lock the actual
    // online state for the duration of the frame,
    // only copy it in a safe way that should support
    // dynamically changing bodies array
    pthread_mutex_lock(&state->mtx);

    vec2 cam_pos = {25.0/2, 25.0/2};
    const float scale_factor = 10.0f;

    vec2 v_o = {0,0};
    vec2 v_x = {1,0};
    vec2 v_y = {0,1};

    float oss_x, oss_y;
    float xss_x, xss_y;
    float yss_x, yss_y;

    wstoss(&oss_x, &oss_y, &v_o, &cam_pos, scale_factor);
    wstoss(&xss_x, &xss_y, &v_x, &cam_pos, scale_factor);
    wstoss(&yss_x, &yss_y, &v_y, &cam_pos, scale_factor);

    float w = scale_factor; float h = scale_factor;

    SDL_FRect origin = {
        .x = oss_x - w/2, .y = oss_y - h/2,
        .w = w, .h = h
    };
    SDL_FRect x = {
        .x = xss_x - w/2, .y = xss_y - h/2,
        .w = w, .h = h
    };
    SDL_FRect y = {
        .x = yss_x - w/2, .y = yss_y - h/2,
        .w = w, .h = h
    };

    SDL_SetRenderDrawColor(renderer, 255,0,0,255);
    SDL_RenderFillRectF(renderer, &origin);
    SDL_SetRenderDrawColor(renderer, 0,255,0,255);
    SDL_RenderFillRectF(renderer, &x);
    SDL_SetRenderDrawColor(renderer, 0,0,255,255);
    SDL_RenderFillRectF(renderer, &y);

    SDL_FRect rect = {
        .x = 0, .y = 0,
        .w = 5.f, .h = 5.f
    };

    SDL_SetRenderDrawColor(renderer, 255,255,255,100);
    for(int i = 0; i < state->sz; i++)
    {
        vec2 *p = &state->bodies[i].pos;

        wstoss(&rect.x, &rect.y, p, &cam_pos, scale_factor);

        SDL_RenderFillRectF(renderer, &rect);
    }

    pthread_mutex_unlock(&state->mtx);
    SDL_RenderPresent(renderer);
}

int main()
{
    int ret = sdl_init();
    if(ret) return ret;

    state_t *state = malloc(sizeof(state_t));
    state_init(state);

    pthread_t simthr;
    ret = pthread_create(
            &simthr,
            0,
            (void *(*)(void *))sim_start,
            (void *)state
            );
    assert(ret == 0 && "could not create simulation thread");

    const float fdt = 1.0/240;

#if 1
    while(state->loop)
    {
        draw(state);
        usleep(fdt*1000000);
    }
#else
    pthread_join(simthr, 0);
#endif

    return 0;
}

