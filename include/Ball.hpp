#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <unordered_map>
#include "../include/Vec2.hpp"
#include <random>
#include <chrono>

class Ball
{
public:
    Ball(const Vec2& pos, const Vec2& velo, int radius, SDL_Renderer* renderer);

    void renderBall(SDL_Renderer* renderer) const;

    const Vec2& getPos() const;
    static std::unordered_map<int, SDL_Texture*> textureCache;
    const Vec2& getVelo() const;
    int getRadius() const;

    void setPos(const Vec2& pos);
    void setVelo(const Vec2& velo);
    void setRadius(int newRadius);
    bool handleCollisionWithLineSegment(const Vec2& p1, const Vec2& p2, double deltaTime);
    bool handleCollisionWithCircle(const Ball& other, double deltaTime);
    static Vec2 getRandomPosVector(int WIDTH, int HEIGHT, int ballRad, int grinderCircleRad, bool excludeMiddle);

    Vec2 computeCollisionVelocity(Vec2& norm);

private:
    int radius;
    Vec2 pos;
    Vec2 velo;
    SDL_Texture* texture;
};
