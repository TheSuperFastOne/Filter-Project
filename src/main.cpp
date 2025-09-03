#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <math.h>

#include "../include/Ball.hpp"
#include "../include/RenderWindow.hpp"
#include "../include/Time.hpp"

void RenderLine(const Vec2& p1, const Vec2& p2, int R, int G, int B, int A, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, R, G, B, A);
    SDL_RenderDrawLine(renderer,
        (int)(p1.getX() * 100), (int)(1200-(p1.getY() * 100)),
        (int)(p2.getX() * 100), (int)(1200-(p2.getY() * 100)));
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Initilization of SDL failed: " << SDL_GetError() << std::endl;
        return 1; // return 1 means program fail
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "Initialization of Images for SDL failed:  " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    float grinderCircleRad = 3; // METERS
    float ballRad = 1.0; // METERS
    float gapDistance = 1.0;

    const int WINDOW_HEIGHT = 1200; // Pixels
    const int WINDOW_WIDTH = (int)((gapDistance + grinderCircleRad*2 + ballRad*2)* 100); // Pixels
    RenderWindow window("Minimal SDL2 Window", WINDOW_WIDTH+1, WINDOW_HEIGHT+1); //Handles lines on the edge really well. Makes no difference to the simulation sooo idgaf

    Ball ball(Vec2(4.5, 11), Vec2(0, 0), ballRad, window.getRenderer());
    
    const double physicsFps = 60; // 180 Frames per Second (expect this number to be really inconsistent I never really update it)
    const double physicsDeltaTime = 1.0 / physicsFps; // However-many seconds per frame i can't be bothered to type that into a fucking calculator
    const double targetMs = 1000.0 / physicsFps; // Milliseconds
    const Uint64 perfFreq = SDL_GetPerformanceFrequency();

    Vec2 gravity(0, -9.8);

    bool running = true;
    SDL_Event event;
    double previousEnergy = 0.0;

    // --- NEW: fixed-step accumulator timing state ---
    double accumulator = 0.0;
    Uint64 prevCounter = SDL_GetPerformanceCounter();

//----------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------//
    while (running)
    {
        const Uint64 frameStart = SDL_GetPerformanceCounter();

        // --- NEW: frame time bookkeeping for accumulator ---
        double frameTime = (frameStart - prevCounter) / (double)perfFreq;
        prevCounter = frameStart;
        if (frameTime > 0.25) frameTime = 0.25; // avoid spiral-of-death on big pauses
        accumulator += frameTime;

         while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }
            }

        // Physics!!
        //Collisions first
        // --- NEW: do fixed-timestep physics in a loop ---
        while (accumulator >= physicsDeltaTime)
        {
            ball.snapshot(); // remember previous state for interpolation

            bool collided1 = ball.handleCollisionWithLineSegment(Vec2(0, 0), Vec2(9, 0), physicsDeltaTime, gravity);
            bool collided2 = ball.handleCollisionWithLineSegment(Vec2(0, 0), Vec2(0, 12), physicsDeltaTime, gravity);
            bool collided3 = ball.handleCollisionWithLineSegment(Vec2(9, 0), Vec2(9, 12), physicsDeltaTime, gravity);
            bool collided = collided1 || collided2 || collided3;

            // Remember the equation deltax = 1/2at^2 + v0t. Here, t = physicsDeltaTime. v0 is the current velocity. a is acceleration due to gravity
            if (!collided)
            {
                Vec2 change_in_pos = (gravity*(physicsDeltaTime)*0.5 + ball.getVelo())*physicsDeltaTime;
                ball.setPos(ball.getPos() + change_in_pos);
                //v = v0 + at
                ball.setVelo(ball.getVelo() + gravity*physicsDeltaTime);
            }

            accumulator -= physicsDeltaTime;
        }

        // --- Energy check (same logic, just using the current state after fixed steps) ---
        double EnergyOfBall = ball.getPos().getY()*9.8 + 0.5*ball.getVelo().magnitude()*ball.getVelo().magnitude();
        double changeInEnergy = EnergyOfBall-previousEnergy;
        if (abs(changeInEnergy) > 5e-12 && changeInEnergy != EnergyOfBall)
        {
            std::cout << "Energy was " << previousEnergy << ", but changed by " << changeInEnergy << std::endl; // alr this isn't really going off which is a really, really good sign
        }
        previousEnergy = EnergyOfBall;

        // --- NEW: render interpolation factor in [0,1) from leftover accumulator ---
        double alpha = accumulator / physicsDeltaTime;
        
        SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255);
        window.clear();




            SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255);
            window.clear();

            // --- NEW: interpolate between previous and current physics states
            Vec2 p0 = ball.getPrevPos();
            Vec2 p1 = ball.getPos();
            Vec2 pInterp = p0 * (1.0 - alpha) + p1 * alpha;

            // ball.renderBall(window.getRenderer()); // old direct render at pos
            ball.renderBallAt(window.getRenderer(), pInterp); // NEW: smooth render at interpolated position

            RenderLine(Vec2(9.0, 0.0), Vec2(9.0, 12.0), 243, 23, 12, 255, window.getRenderer());
            RenderLine(Vec2(0.0, 0.0), Vec2(9.0, 0.0), 243, 23, 12, 255, window.getRenderer());
            RenderLine(Vec2(0.0, 0.0), Vec2(0.0, 12.0), 243, 23, 12, 255, window.getRenderer());

            window.display();



        // 1/FPS cap
        for (;;) 
        {
            double elapsedMs = (SDL_GetPerformanceCounter() - frameStart) * 1000.0 / (double)perfFreq;
            double remainMs  = targetMs - elapsedMs;
            if (remainMs <= 0.0) break;

            if (remainMs > 1.0) 
            {
                SDL_Delay((Uint32)(remainMs - 0.5)); // keep a small margin
            } 
            else 
            {
            }
        }
    }
}
