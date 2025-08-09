#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

class Vec2
{
    public:
        Vec2(double x, double y);
        double getX() const;
        double getY() const;
        Vec2 operator+(const Vec2& other) const;
        Vec2 operator-(const Vec2& other) const;
        Vec2 operator*(const double multiplier) const;
        Vec2 operator/(const double divider) const;


        void setX(double newX);
        void setY(double newY);

        double magnitude() const;

        double dot(const Vec2& other) const;

        Vec2 getUnitVector() const;

    private:
        double x;
        double y;
    
    
};