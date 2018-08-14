#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class Window
{
public:
    SDL_Window* window;
    SDL_GLContext context;

    Window();
    virtual ~Window();
    void Window_update();
};