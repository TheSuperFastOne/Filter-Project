#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

class RenderWindow {
public:
    RenderWindow(const char* title, int width, int height);
    ~RenderWindow();                    // declared
    void clear();
    void display();
    SDL_Renderer* getRenderer() const { return renderer; }
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};