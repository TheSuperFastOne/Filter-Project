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
        Ball(const Vec2& pos, const Vec2& velo, float radius, SDL_Renderer* renderer);
        void renderBall(SDL_Renderer* renderer) const;
        bool handleCollisionWithLineSegment(const Vec2& p1,
                                    const Vec2& p2,
                                    double deltaTime,
                                    const Vec2& gravity);


        // These functions are so unworthy they get an implementation in a header file
        const Vec2& getPos() const {return pos;};
        void snapshot() { prevPos = pos; prevVelo = velo; }
        const Vec2& getPrevPos() const { return prevPos; }
        void renderBallAt(SDL_Renderer* r, const Vec2& worldPos) const;
        const Vec2& getVelo() const {return velo;};
        float getRadius() const {return radius;};

        void setPos(const Vec2& newpos) {pos = newpos;};
        void setVelo(const Vec2& newvelo) {velo = newvelo;};
        void setRadius(float newRadius) {radius = newRadius;};
        //end of unworthy functions
    private:
        Vec2 prevPos, prevVelo;
        float radius;
        Vec2 pos;
        Vec2 velo;
};