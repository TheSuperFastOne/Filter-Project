#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "../include/Vec2.hpp"

class Ball
{
public:
    Ball(const Vec2& pos, const Vec2& velo, int radius);

    void renderBall(SDL_Renderer* renderer) const;

    const Vec2& getPos() const;
    const Vec2& getVelo() const;
    int getRadius() const;

    void setPos(const Vec2& pos);
    void setVelo(const Vec2& velo);
    void setRadius(int newRadius);
    bool handleCollisionWithLineSegment(const Vec2& p1, const Vec2& p2, double deltaTime);
    bool handleCollisionWithCircle(const Ball& other, double deltaTime);

    Vec2 computeCollisionVelocity(Vec2& norm);

private:
    int radius;
    Vec2 pos;
    Vec2 velo;
};
