#include "../include/Ball.hpp"

inline double clamp(double x, double minVal, double maxVal)
{
    return (x < minVal) ? minVal : (x > maxVal ? maxVal : x);
}

Ball::Ball(const Vec2& pos, const Vec2& velo, float radius, SDL_Renderer* renderer) : pos(pos), velo(velo), radius(radius)
{
    // Nothing for now the constructor does everything basically
}

void Ball::renderBall(SDL_Renderer* renderer) const
{
    //Midpoint Line Algorithm, won't be covered in the video but it's a really nice algorithm. Watch here: https://www.youtube.com/watch?v=hpiILbMkF9w
    Vec2 centerPos(pos.getX()*100, 1200 - pos.getY()*100); // Changes coordinate systems, in pixels now
    int radiusPixels = (int)(radius * 100);

    int cx = centerPos.getX();
    int cy = centerPos.getY();
    int r = radiusPixels;

    int dx = 0;
    int dy = r;
    int d = 1 - r;

    SDL_SetRenderDrawColor(renderer, 218, 31, 15, 255);

    while (dx <= dy)
    {
        SDL_RenderDrawPoint(renderer, cx + dy, cy + dx);
        SDL_RenderDrawPoint(renderer, cx - dx, cy + dy);
        SDL_RenderDrawPoint(renderer, cx - dy, cy + dx);
        SDL_RenderDrawPoint(renderer, cx + dx, cy - dy);
        SDL_RenderDrawPoint(renderer, cx + dy, cy - dx);
        SDL_RenderDrawPoint(renderer, cx - dx, cy - dy);
        SDL_RenderDrawPoint(renderer, cx - dy, cy - dx);
        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);

        if (d < 0) {
            d += 2 * dx + 3;
        } else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
    }

}


bool Ball::handleCollisionWithLineSegment(const Vec2& p1, const Vec2& p2, double deltaTime)
{
    const Vec2 a = pos - p1;
    const Vec2 b = p2-p1;

    //Vec2 AR = a - v*(a.dot(v)/v.dot(v)); ### This works for lines, not line segments
    // Any point on the line segment can be written as p1 + t(p2-p1) for 0<=t<=1
    // Maths covered in video explains that...
    const double b2 = b.dot(b);
    // alright technically this doesn't work for b2 equals zero but JUST DONT CALL IT FOR p1 = p2!!
    const double inv_b2 = 1/b2;
    double t = a.dot(b) * inv_b2;
    t = clamp(t, 0.0, 1.0);

    Vec2 n = pos - (p1 + b*t); // This is the vector pointing from the closest point on the line segment to the center of the ball
    const double d2 = n.dot(n);
    const double r2 = radius * radius;

    if (d2 >= r2 || velo.dot(n) >= 0.0) return false;

    const double d = std::sqrt(d2);
    const double penetration = radius - d; // How many pixels did it glitch through kinda
    const double timeBack = std::min(penetration*d/(-velo.dot(n)), deltaTime);

    pos = pos - velo*timeBack;

    // Recompute Everything at new position
    {
        const Vec2 a2 = pos - p1;
        double t2 = (b2 > 0.0) ? a2.dot(b) * inv_b2 : 0.0;
        t2 = clamp(t2, 0.0, 1.0);
        const Vec2 A2 = p1 + b * t2;
        n = pos - A2;                                    
    }

    const double n2 = std::max(n.dot(n), 1e-24); // small epsilon yk like in epsilon delta proofs for limits n shit
    const double vDotN = velo.dot(n);
    if (vDotN >= 0.0) return false;
    velo = velo - n * (2 * vDotN / n2);

    const double remaining = deltaTime - timeBack;
    if (remaining > 0.0) pos = pos + velo * remaining;
    return true;
}



