#include "../include/RenderWindow.hpp"
#include <cmath>

RenderWindow::RenderWindow(const char* title, int width, int height)
    : window(nullptr), renderer(nullptr)
{
    // 1) Nearest-neighbor scaling (no filtering “shading”)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    // 2) High-DPI aware (prevents fractional scaling blur on macOS)
    const Uint32 winFlags = SDL_WINDOW_SHOWN;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, winFlags);

    if (!window) {
        std::cerr << "Window initialization failed: " << SDL_GetError() << std::endl;
    }

    // 3) Accelerated renderer; disable blending unless you need it
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer initialization failed: " << SDL_GetError() << std::endl;
    }

    // 4) Optional: make pixel math exact by locking a logical size
    // SDL_RenderSetLogicalSize(renderer, width, height);
    // SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
}

void RenderWindow::clear()
{
    // Set an explicit opaque clear color BEFORE clearing (prevents faint edge)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void RenderWindow::display()
{
    SDL_RenderPresent(renderer);
}

RenderWindow::~RenderWindow() {
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
}
