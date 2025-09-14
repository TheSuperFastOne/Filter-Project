#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <fstream>
#include <thread>
#include <vector>
#include <fstream>
#include <random>
#include <mutex>

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

std::random_device rd;
std::mt19937 gen(rd());
std::ofstream stuckFile("output.txt", std::ios::app);
Vec2 getRandomPositionVector(float minX, float maxX, float minY, float maxY)
{
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    return Vec2(distX(gen), distY(gen));

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
    float gapDistance = 0.5;


    bool testing = false;
    Vec2 wantToTest = Vec2(5.34329, 9.84982); // For testing individual cases
    Vec2 spawnedPos = wantToTest;
    if (!testing)
    {
        // If i'm not testing, spawn randomly
        Vec2 spawnedPos = getRandomPositionVector(ballRad, grinderCircleRad*2+gapDistance+ballRad, 8.5, 11.0); // MaxX is the same as WORLD_WIDTH - ballRad.
    }


    const int WINDOW_HEIGHT = 1200; // Pixels
    const double WORLD_WIDTH = gapDistance + grinderCircleRad*2 + ballRad*2; // Meters
    const int WINDOW_WIDTH = (int)(WORLD_WIDTH*100); // Pixels
    RenderWindow window("Minimal SDL2 Window", WINDOW_WIDTH+1, WINDOW_HEIGHT+1); //Handles lines on the edge really well. Makes no difference to the simulation sooo idgaf
    SDL_RenderSetVSync(window.getRenderer(), 1); // Enable VSync to prevent screen tearing

    Ball ball(spawnedPos, Vec2(0, 0), ballRad, window.getRenderer());
    
    const double physicsFps = 1500; // 180 Frames per Second (expect this number to be really inconsistent I never really update it)
    const double physicsDeltaTime = 1.0 / physicsFps; // However-many seconds per frame i can't be bothered to type that into a fucking calculator
    const double targetMs = 1000.0 / physicsFps; // Milliseconds
    const Uint64 perfFreq = SDL_GetPerformanceFrequency();



    const double renderFps = 60;
    const double renderDeltaTime = 1.0 / renderFps;
    const double renderTargetMs = 1000.0 / renderFps;

    Vec2 gravity(0, -9.8);
    bool render = false; // If false, don't render and also, don't wait at all.

    bool running = true;
    SDL_Event event;
    double previousEnergy = 0.0;
    int bounces = 0;

    int trials_num = 2147000000; // Goddamn thats big im js tryna get some data yk
    int trial = 1;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------//
Uint64 lastCounter = SDL_GetPerformanceCounter();
double accumulator = 0.0;
Ball grinder1(Vec2(0,4.5), Vec2(0, 0), grinderCircleRad, window.getRenderer());
Ball grinder2(Vec2(WORLD_WIDTH,4.5), Vec2(0, 0), grinderCircleRad, window.getRenderer());
    while (running)
    {
        {//ScopedTimer timer("Main loop");
        Uint64 now = SDL_GetPerformanceCounter();
        double frameSec = (now - lastCounter) / (double)perfFreq;
        lastCounter = now;

        if (frameSec > 0.25) frameSec = 0.25;

        accumulator += frameSec;
        //std::cout << "pos = (" << ball.getPos().getX() << ", " << ball.getPos().getY() << ")\n";
         while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }
            }
        // Physics!!
        //Collisions first
        //{ScopedTimer timer("Physics"); // This is just for profiling, ignore it
        while (accumulator >= physicsDeltaTime || !render)
        {
            // Save previous state for interpolation
            ball.snapshot();

            //Collisions first at fixed dt
            bool collided1 = ball.handleCollisionWithLineSegment(Vec2(0, 0), Vec2(WORLD_WIDTH, 0), physicsDeltaTime, gravity);
            bool collided2 = ball.handleCollisionWithLineSegment(Vec2(0, 0), Vec2(0, 12), physicsDeltaTime, gravity);
            bool collided3 = ball.handleCollisionWithLineSegment(Vec2(WORLD_WIDTH, 0), Vec2(WORLD_WIDTH, 12), physicsDeltaTime, gravity);
            bool collided4 = ball.handleCollisionWithCircle(grinder1, physicsDeltaTime, gravity);
            bool collided5 = ball.handleCollisionWithCircle(grinder2, physicsDeltaTime, gravity);
            if (collided4 || collided5) {
                bounces++;
                if (bounces > 1500)
                {
                    std::cout << "Goddamn it ball got stuck\n";
                    stuckFile << "Stuck ball at (" << spawnedPos.getX() << ", " << spawnedPos.getY() << ")\n";
                    stuckFile.flush();
                    bounces = 0;
                trial++;
                if (trial > trials_num)
                {
                    running = false;
                    std::cout << "All trials complete\n";
                    break;
                }
                spawnedPos = getRandomPositionVector(ballRad, WORLD_WIDTH-(ballRad), 8.5, 11.0);
                //std::cout << "New ball at (" << spawnedPos.getX() << ", " << spawnedPos.getY() << ")\n";
                ball.setPos(spawnedPos); // Has to be more than 8.5 to not start in a grinder
                ball.setVelo(Vec2(0, 0));
                    continue;
                }
                //
            }
            if (collided1)
            {
                //std::cout << bounces << " bounces on trial " << trial << "\n";
                bounces = 0;
                trial++;
                if (trial > trials_num)
                {
                    running = false;
                    std::cout << "All trials complete\n";
                    break;
                }
                spawnedPos = getRandomPositionVector(ballRad, WORLD_WIDTH-(ballRad), 8.5, 11.0);
                //std::cout << "New ball at (" << spawnedPos.getX() << ", " << spawnedPos.getY() << ")\n";
                ball.setPos(spawnedPos); // Has to be more than 8.5 to not start in a grinder
                ball.setVelo(Vec2(0, 0));
            }
            bool collided = collided1 || collided2 || collided3 || collided4 || collided5;

            // Energy check
            double EnergyOfBall = ball.getPos().getY()*9.8 + 0.5*ball.getVelo().magnitude()*ball.getVelo().magnitude();
            double changeInEnergy = EnergyOfBall - previousEnergy;
            if ((std::abs(changeInEnergy) > 5e-12 && changeInEnergy != EnergyOfBall))
            {
                //std::cout << "Energy was " << previousEnergy << ", but changed by " << changeInEnergy << std::endl;
            }
            previousEnergy = EnergyOfBall;

            // Integrate one fixed step if no collision consumed it
            if (!collided)
            {
                Vec2 change_in_pos = (gravity*(physicsDeltaTime)*0.5 + ball.getVelo())*physicsDeltaTime; // 0.5 a dt^2 + v dt
                ball.setPos(ball.getPos() + change_in_pos);
                ball.setVelo(ball.getVelo() + gravity*physicsDeltaTime); // v = v0 + a dt
            }

            accumulator -= physicsDeltaTime;
        }
        //}
            if (render){//ScopedTimer timer("Render"); // This is just for profiling, ignore it

            double alpha = accumulator / physicsDeltaTime; // in [0,1)
            Vec2 interpPos = ball.getPrevPos() * (1.0 - alpha) + ball.getPos() * alpha;

            SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255);
            window.clear();


            ball.renderBallAt(window.getRenderer(), interpPos);
            grinder1.renderBall(window.getRenderer());
            grinder2.renderBall(window.getRenderer());


            RenderLine(Vec2(WORLD_WIDTH, 0.0), Vec2(WORLD_WIDTH, 12.0), 243, 23, 12, 255, window.getRenderer());
            RenderLine(Vec2(0.0, 0.0), Vec2(WORLD_WIDTH, 0.0), 243, 23, 12, 255, window.getRenderer());
            RenderLine(Vec2(0.0, 0.0), Vec2(0.0, 12.0), 243, 23, 12, 255, window.getRenderer());

            window.display();
            }


        // 1/FPS cap
        { 
        //ScopedTimer timer("FPS cap");
        if (render){
        for (;;) 
        {
            double frameMs = (SDL_GetPerformanceCounter() - now) * 1000.0 / (double)perfFreq;
            double remainMs = renderTargetMs - frameMs;
            if (remainMs <= 0.0) break;

            if (remainMs > 1.0) {
                SDL_Delay((Uint32)(remainMs - 0.5)); // bulk sleep
            } else {
                // busy-wait the last fraction for more precise pacing
                while (((SDL_GetPerformanceCounter() - now) * 1000.0 / (double)perfFreq) < renderTargetMs) { }
                break;
            }
        }
    }
    }
    }
    }
    stuckFile.close();
}

