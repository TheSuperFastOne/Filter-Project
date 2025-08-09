#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

class RenderWindow
{
public:
    RenderWindow(const char* title, int width, int height);
    ~RenderWindow();

    void clear();
    void display();
    void cleanUp();

    SDL_Renderer* getRenderer() { return renderer; }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};