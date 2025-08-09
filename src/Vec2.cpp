#include"../include/Vec2.hpp"

Vec2::Vec2(double x, double y) : x(x), y(y) {}

double Vec2::getX() const
{
    return this->x;
}

double Vec2::getY() const
{
    return this->y;
}

void Vec2::setX(double newX)
{
    x = newX;
}

void Vec2::setY(double newY)
{
    y = newY;
}

double Vec2::magnitude() const
{
    return std::sqrt(x * x + y * y);
}

double Vec2::dot(const Vec2& other) const
{
    return x * other.x + y * other.y;
}

Vec2 Vec2::getUnitVector() const
{
    double magnitude = this->magnitude();
    if (magnitude==0)
    {
        std::cout << "Attempted to get unit vector of 0,0 vector\n";
        return Vec2(0,0);
    }
    Vec2 vector = Vec2(x/magnitude, y/magnitude);
    return vector;
}

Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(const double multiplier) const
{
    return Vec2(x * multiplier, y * multiplier);
}

Vec2 Vec2::operator/(const double divider) const
{
    return Vec2(x / divider, y / divider);
}
