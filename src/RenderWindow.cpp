#include "../include/RenderWindow.hpp"

RenderWindow::RenderWindow(const char* title, int width, int height)
    : window(nullptr), renderer(nullptr)
{
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Window initialization failed: " << SDL_GetError() << std::endl;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Renderer initialization failed: " << SDL_GetError() << std::endl;
    }
}

RenderWindow::~RenderWindow()
{
    cleanUp();
}

void RenderWindow::clear()
{
    SDL_RenderClear(renderer);
}

void RenderWindow::display()
{
    SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}
