#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#include "../include/Ball.hpp"
#include "../include/RenderWindow.hpp"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    RenderWindow window("Minimal SDL2 Window", 800, 1200);

    bool running = true;
    SDL_Event event;
    bool slowingdown = true;

    Ball ball(Vec2(600,110), Vec2(0,0), 100);

    Ball grinder1(Vec2(0,750), Vec2(0,0), 280);
    Ball grinder2(Vec2(800,750), Vec2(0,0), 280);

    int bounces = 0;

    const double PIXELS_PER_METER = 60;

    //Acceleration vector
    Vec2 gravity(0,9.8 * PIXELS_PER_METER);

    const double physicsFps = 150.0;
    const double physicsDeltaTime = 1.0 / physicsFps;
    const Uint32 frameDelay = static_cast<Uint32>(1000.0 / physicsFps); // in milliseconds

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    while (running)
    {
        Uint64 frameStart = SDL_GetTicks();

        last = now;
        now = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Clear screen
        SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255);
        window.clear();

        // Physics update with fixed timestep
        ball.setVelo(ball.getVelo() + gravity * physicsDeltaTime);
        ball.setPos(ball.getPos() + ball.getVelo() * physicsDeltaTime);

        // Collisions
        ball.handleCollisionWithLineSegment(Vec2(800,600), Vec2(800,0), physicsDeltaTime);
        ball.handleCollisionWithLineSegment(Vec2(0,600), Vec2(0,0), physicsDeltaTime);

        if (ball.handleCollisionWithCircle(grinder2, physicsDeltaTime))
        {
            bounces++;
            std::cout << "Current bounces: " << bounces << std::endl;
        }
        if (ball.handleCollisionWithCircle(grinder1, physicsDeltaTime))
        {
            bounces++;
            std::cout << "Current bounces: " << bounces << std::endl;
        }

        if (ball.handleCollisionWithLineSegment(Vec2(0,1200), Vec2(800,1200), physicsDeltaTime))
        {
            running = false;
            std::cout << "Bounces before hitting bottom: " << bounces << std::endl;
        }

        // Rendering balls
        SDL_SetRenderDrawColor(window.getRenderer(), 214, 23, 57, 255);
        ball.renderBall(window.getRenderer());
        grinder1.renderBall(window.getRenderer());
        grinder2.renderBall(window.getRenderer());

        window.display();

        // Frame timing
        Uint32 frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime && slowingdown)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    window.cleanUp();
    return 0;
}
