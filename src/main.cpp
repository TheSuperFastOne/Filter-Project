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

    Ball ball(Vec2(600,110), Vec2(0,0), 100);

    Ball grinder1(Vec2(0,750), Vec2(0,0), 280);
    Ball grinder2(Vec2(800,750), Vec2(0,0), 280);

    int bounces = 0;

    const double PIXELS_PER_METER = 60;

    //Acceleration vector
    Vec2 gravity(0,9.8 * PIXELS_PER_METER);

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double deltaTime = 0;



    while (running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false; // game loop die :(
        }

        SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255); // Background color is black
        window.clear(); // Clears entire screen

        deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency()) / 1000.0;
        //Move Balls / Physics
        ball.setVelo(ball.getVelo() + gravity*deltaTime);
        ball.setPos(ball.getPos()+ball.getVelo()*deltaTime);

        //Collision
        ball.handleCollisionWithLineSegment(Vec2(800,600), Vec2(800,0), deltaTime);
        ball.handleCollisionWithLineSegment(Vec2(0,600), Vec2(0,0), deltaTime);

        if (ball.handleCollisionWithCircle(grinder2, deltaTime))
        {
            bounces++;
            std::cout << "Current bounces: " << bounces << std::endl;
        }
        if (ball.handleCollisionWithCircle(grinder1, deltaTime))
        {
            bounces++;
            std::cout << "Current bounces: " << bounces << std::endl;
        }

        if (ball.handleCollisionWithLineSegment(Vec2(0,1200), Vec2(800,1200), deltaTime))
        {
            running = false;
            std::cout << "Bounces before hitting bottom: " << bounces << std::endl;
        }



        

        //Rendering done here
        SDL_SetRenderDrawColor(window.getRenderer(), 214, 23, 57, 255); 
        ball.renderBall(window.getRenderer());
        grinder1.renderBall(window.getRenderer());
        grinder2.renderBall(window.getRenderer());


        //Baked onto the display essentially
        window.display();

    }

    window.cleanUp();
    return 0;
}
