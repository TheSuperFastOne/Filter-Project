#include "../include/Ball.hpp"

Ball::Ball(const Vec2& pos, const Vec2& velo, int radius)
    : pos(pos), velo(velo), radius(radius) {}

// Getters returning const references or values
const Vec2& Ball::getPos() const {
    return pos;
}

const Vec2& Ball::getVelo() const {
    return velo;
}

int Ball::getRadius() const {
    return radius;
}

// Setters
void Ball::setPos(const Vec2& newPos) {
    pos = newPos;
}

void Ball::setVelo(const Vec2& newVelo) {
    velo = newVelo;
}

void Ball::setRadius(int newRadius) {
    radius = newRadius;
}

// Render the ball using Midpoint circle algorithm
void Ball::renderBall(SDL_Renderer* renderer) const
{
    int cx = static_cast<int>(pos.getX());
    int cy = static_cast<int>(pos.getY());
    int r = radius;

    int dx = 0;
    int dy = r;
    int d = 1 - r;

    while (dx <= dy)
    {
        // Draw all eight octants
        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
        SDL_RenderDrawPoint(renderer, cx + dy, cy + dx);
        SDL_RenderDrawPoint(renderer, cx - dx, cy + dy);
        SDL_RenderDrawPoint(renderer, cx - dy, cy + dx);
        SDL_RenderDrawPoint(renderer, cx + dx, cy - dy);
        SDL_RenderDrawPoint(renderer, cx + dy, cy - dx);
        SDL_RenderDrawPoint(renderer, cx - dx, cy - dy);
        SDL_RenderDrawPoint(renderer, cx - dy, cy - dx);

        if (d < 0) {
            d += 2 * dx + 3;
        } else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
    }
}

Vec2 Ball::computeCollisionVelocity(Vec2& norm)
{
    Vec2 u = norm.getUnitVector(); // Normalize
    double scalarMult = 2 * velo.dot(u);
    return velo - (u * scalarMult);
}

bool Ball::handleCollisionWithLineSegment(const Vec2& p1, const Vec2& p2, double deltaTime)
{
    Vec2 ballCenter = pos;
    Vec2 segVector = p2 - p1;
    Vec2 lineDir = segVector.getUnitVector();

    Vec2 p1ToCenter = ballCenter - p1;

    // Project p1ToCenter onto lineDir (scalar projection)
    double projLength = p1ToCenter.dot(lineDir);

    // Clamp projection to lie on the segment
    projLength = std::fmax(0.0, std::fmin(projLength, segVector.magnitude()));

    // Closest point on segment to ball center
    Vec2 closestPoint = p1 + lineDir * projLength;

    // Vector from closest point to ball center (perpendicular vector)
    Vec2 diff = ballCenter - closestPoint;
    double dist = diff.magnitude();

    if (dist < radius)
    {
        Vec2 norm = diff.getUnitVector(); // collision normal

        double penetration = radius - dist;

        double velocityAlongNorm = velo.dot(norm);

        if (velocityAlongNorm < 0) // ball moving toward line segment
        {
            double timeBack = penetration / (-velocityAlongNorm);

            // rewind position
            pos = pos - velo * timeBack;

            // reflect velocity along normal with restitution
            Vec2 vprime = velo - norm * (2 * velo.dot(norm));
            velo = vprime;

            // advance remaining time
            double remainingTime = deltaTime - timeBack;
            pos = pos + velo * remainingTime;

            return true;
        }
    }
    return false;
}

bool Ball::handleCollisionWithCircle(const Ball& other, double deltaTime)
{
    Vec2 ballCenter = pos;
    Vec2 otherCenter = other.getPos();

    Vec2 diff = ballCenter - otherCenter;
    double radiusSum = radius + other.getRadius();
    double dist = diff.magnitude();

    if (dist < radiusSum)
    {
        Vec2 norm = diff.getUnitVector(); // normal from other to ball

        double penetration = radiusSum - dist;

        double velocityAlongNorm = velo.dot(norm);

        if (velocityAlongNorm < 0) // ball moving toward other
        {
            double timeBack = penetration / (-velocityAlongNorm);

            // rewind position to moment of impact
            pos = pos - velo * timeBack;

            // reflect velocity vector along collision normal and apply restitution
            Vec2 vprime = velo - norm * (2 * velo.dot(norm));
            velo = vprime;

            // advance position for remaining frame time
            double remainingTime = deltaTime - timeBack;
            pos = pos + velo * remainingTime;

            return true;
        }
    }
    return false;
}


