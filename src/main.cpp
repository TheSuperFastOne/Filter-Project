#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

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

    // Define the range of GAP_DISTANCE_MINUS_BALL_DIAMETER values you want to test
    std::vector<int> gap_values = {50}; // Add as many as you want
    bool running = true;
    for (int GAP_DISTANCE_MINUS_BALL_DIAMETER : gap_values)
    {
        int TRIALS_NUM = 5;
        std::vector<int> trials(TRIALS_NUM);
        int grinderCircleRad = 280;
        int ballRad = 100;
        bool excludeMiddle = false;

        

        Uint64 start = SDL_GetPerformanceCounter();
        const int HEIGHT = 1200;
        int half_height = HEIGHT / 2;
        const int WIDTH = GAP_DISTANCE_MINUS_BALL_DIAMETER + grinderCircleRad*2 + 2*ballRad;
        RenderWindow window("Minimal SDL2 Window", WIDTH, HEIGHT);

        bool sim_running = true;
        bool render = true;
        SDL_Event event;
        bool slowingdown = true;

        Ball ball(Ball::getRandomPosVector(WIDTH, HEIGHT, ballRad, grinderCircleRad), Vec2(0,0), ballRad, window.getRenderer());
        Ball grinder1(Vec2(0,750), Vec2(0,0), grinderCircleRad, window.getRenderer());
        Ball grinder2(Vec2(WIDTH,750), Vec2(0,0), grinderCircleRad, window.getRenderer());

        int bounces = 0;
        const double PIXELS_PER_METER = 60;
        Vec2 gravity(0,9.8 * PIXELS_PER_METER);

        const double physicsFps = 150;
        const double physicsDeltaTime = 1.0 / physicsFps;
        const Uint32 frameDelay = static_cast<Uint32>(1000.0 / physicsFps);

        Uint64 now = SDL_GetPerformanceCounter();
        Uint64 last = 0;
        SDL_SetRenderDrawColor(window.getRenderer(), 214, 23, 57, 255);
        int trial = 1;
        while (sim_running)
        {
            Uint64 frameStart = SDL_GetTicks();

            last = now;
            now = SDL_GetPerformanceCounter();

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                    sim_running = false;
                }
            }
            if (!running || !sim_running)
            {
                break;
            }

            SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 255);
            window.clear();

            ball.setVelo(ball.getVelo() + gravity * physicsDeltaTime);
            ball.setPos(ball.getPos() + ball.getVelo() * physicsDeltaTime);

            ball.handleCollisionWithLineSegment(Vec2(WIDTH,half_height), Vec2(WIDTH,0), physicsDeltaTime);
            ball.handleCollisionWithLineSegment(Vec2(0,half_height), Vec2(0,0), physicsDeltaTime);

            if (ball.handleCollisionWithCircle(grinder2, physicsDeltaTime))
            {
                bounces++;
            }
            if (ball.handleCollisionWithCircle(grinder1, physicsDeltaTime))
            {
                bounces++;
            }

            if (ball.handleCollisionWithLineSegment(Vec2(0,1200), Vec2(800,1200), physicsDeltaTime))
            {
                if (trial == TRIALS_NUM)
                {
                    sim_running = false;
                }
                trials[trial-1] = bounces;
                bounces = 0;
                if (sim_running)
                {
                    Vec2 randomvec = Ball::getRandomPosVector(WIDTH, HEIGHT, ballRad, grinderCircleRad);
                    ball.setPos(randomvec);
                    ball.setVelo(Vec2(0,0));
                    trial++;
                }
                
            }

            if (render)
            {
                ball.renderBall(window.getRenderer());
                grinder1.renderBall(window.getRenderer());
                grinder2.renderBall(window.getRenderer());
                window.display();
            }

            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameDelay > frameTime && slowingdown)
            {
                SDL_Delay(frameDelay - frameTime);
            }
        }

        window.cleanUp();
        Uint64 totalTimeTaken = SDL_GetPerformanceCounter() - start;
        double elapsedSeconds = static_cast<double>(totalTimeTaken) / SDL_GetPerformanceFrequency();
        std::cout << "GAP_DISTANCE_MINUS_BALL_DIAMETER = " << GAP_DISTANCE_MINUS_BALL_DIAMETER << "\n";
        for (int i = 0; i < TRIALS_NUM; i++) {
            std::cout << "Trial " << i+1 << ": " << trials[i] << " bounces\n";
        }
        std::cout << "Total time taken: " << elapsedSeconds << " seconds" << std::endl;
        std::cout << "-----------------------------\n";
        if (!running)
        {
            break;
        }
    }
    IMG_Quit();
    SDL_Quit();
    return 0;
}


