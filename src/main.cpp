#include <iostream>
#include <fstream>
#include <random>
#include <math.h>
#include <thread>
#include <vector>

#include "../include/Ball.hpp"

std::vector<std::pair<double, double>> load_points(const std::string& filename) {
    std::vector<std::pair<double, double>> points;
    std::ifstream f(filename);
    std::string line;
    while (std::getline(f, line)) {
        if (line.find("Stuck ball at") != std::string::npos) {
            auto l = line.find('('), r = line.find(')');
            auto comma = line.find(',', l);
            double x = std::stod(line.substr(l+1, comma-l-1));
            double y = std::stod(line.substr(comma+1, r-comma-1));
            points.emplace_back(x, y);
        }
    }
    return points;
}

Vec2 getRandomPositionVector(std::mt19937& gen, float minX, float maxX, float minY, float maxY) {
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);
    return Vec2(distX(gen), distY(gen));
}

// Smart random position generator (around points from dataset)
Vec2 getSmartRandomPositionVector(
    std::mt19937& gen,
    const std::vector<std::pair<double, double>>& points,
    float minX, float maxX, float minY, float maxY
) {
    std::uniform_int_distribution<size_t> pick_point(0, points.size() - 1);
    std::uniform_real_distribution<double> dist_d(0.0, 0.015625);
    std::uniform_real_distribution<double> dist_theta(0.0, 2 * M_PI);

    auto [x_ref, y_ref] = points[pick_point(gen)];
    double d = dist_d(gen);
    double theta = dist_theta(gen);
    double x_new = x_ref + d * std::cos(theta);
    double y_new = y_ref + d * std::sin(theta);

    // Clamp to bounds
    x_new = std::max(static_cast<double>(minX), std::min(x_new, static_cast<double>(maxX)));
    y_new = std::max(static_cast<double>(minY), std::min(y_new, static_cast<double>(maxY)));

    return Vec2(x_new, y_new);
}

void run_trials(int thread_id, int trials_num, float grinderCircleRad, float ballRad, float gapDistance, double WORLD_WIDTH) {
    std::ofstream stuckFile("output_thread" + std::to_string(thread_id) + ".txt", std::ios::app);

    std::random_device rd;
    std::mt19937 gen(rd() + thread_id);
    

    Vec2 gravity(0, -9.8);
    const double physicsFps = 1500;
    const double physicsDeltaTime = 1.0 / physicsFps;
    std::vector<std::pair<double, double>> stuck_points = load_points("merged_stuck_points.txt");

    int trial = 1;
    while (trial <= trials_num) {
        Vec2 spawnedPos = getSmartRandomPositionVector(gen, stuck_points, ballRad, grinderCircleRad*2+gapDistance+ballRad, 8.5, 11.0);
        //Vec2 spawnedPos = getRandomPositionVector(gen, ballRad, grinderCircleRad*2+gapDistance+ballRad, 8.5, 11.0);
        Ball ball(spawnedPos, Vec2(0, 0), ballRad, nullptr);

        int bounces = 0;
        double previousEnergy = 0.0;
        bool trial_done = false;

        Ball grinder1(Vec2(0,4.5), Vec2(0, 0), grinderCircleRad, nullptr);
        Ball grinder2(Vec2(WORLD_WIDTH,4.5), Vec2(0, 0), grinderCircleRad, nullptr);

        while (!trial_done) {
            ball.snapshot();

            bool collided1 = ball.handleCollisionWithLineSegment(Vec2(0, 0), Vec2(WORLD_WIDTH, 0), physicsDeltaTime, gravity);
            bool collided2 = ball.handleCollisionWithLineSegment(Vec2(0, 0), Vec2(0, 12), physicsDeltaTime, gravity);
            bool collided3 = ball.handleCollisionWithLineSegment(Vec2(WORLD_WIDTH, 0), Vec2(WORLD_WIDTH, 12), physicsDeltaTime, gravity);
            bool collided4 = ball.handleCollisionWithCircle(grinder1, physicsDeltaTime, gravity);
            bool collided5 = ball.handleCollisionWithCircle(grinder2, physicsDeltaTime, gravity);

            if (collided4 || collided5) {
                bounces++;
                if (bounces > 1500) {
                    stuckFile << "Stuck ball at (" << spawnedPos.getX() << ", " << spawnedPos.getY() << ")\n";
                    stuckFile.flush();
                    trial_done = true;
                    continue;
                }
            }
            if (collided1) {
                trial_done = true;
                continue;
            }

            bool collided = collided1 || collided2 || collided3 || collided4 || collided5;

            double EnergyOfBall = ball.getPos().getY()*9.8 + 0.5*ball.getVelo().magnitude()*ball.getVelo().magnitude();
            double changeInEnergy = EnergyOfBall - previousEnergy;
            previousEnergy = EnergyOfBall;

            if (!collided) {
                Vec2 change_in_pos = (gravity*(physicsDeltaTime)*0.5 + ball.getVelo())*physicsDeltaTime;
                ball.setPos(ball.getPos() + change_in_pos);
                ball.setVelo(ball.getVelo() + gravity*physicsDeltaTime);
            }
        }
        trial++;
    }
    stuckFile.close();
}

int main(int argc, char* argv[])
{
    float grinderCircleRad = 3; // METERS
    float ballRad = 1.0; // METERS
    float gapDistance = 0.5;
    double WORLD_WIDTH = gapDistance + grinderCircleRad*2 + ballRad*2;
    int trials_num = 2147000000; // Total trials you want

    int num_threads = std::thread::hardware_concurrency(); // Or set manually
    int trials_per_thread = trials_num / num_threads;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(run_trials, i, trials_per_thread, grinderCircleRad, ballRad, gapDistance, WORLD_WIDTH);
    }
    for (auto& t : threads) t.join();

    return 0;
}