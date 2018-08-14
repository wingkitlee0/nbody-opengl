#pragma once

#include <stdbool.h>
#include <vector>
#include "window.hpp"
#include "player.hpp"
#include "star.hpp"

#define NUM_STARS 200

class NBody
{
public:
    Window* window;
    Player* player;
    bool running;
    //Star stars[NUM_STARS];
    std::vector<Star> stars;
    GLuint star_texture;

    NBody();
    ~NBody();
    void NBody_run();

    // sdl stuff
    void NBody_handle_event(SDL_Event event);
    void NBody_handle_events();
    void NBody_tick(uint32_t dt);
    void NBody_draw_star(Star star);
    void NBody_draw_stars();
    void NBody_draw();
    void NBody_step(uint32_t dt);
};