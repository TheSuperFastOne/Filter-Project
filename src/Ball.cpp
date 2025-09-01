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


bool Ball::handleCollisionWithLineSegment(const Vec2& p1,
                                         const Vec2& p2,
                                         double deltaTime,
                                         const Vec2& gravity)
{
    // Vector from p1 to current center, and segment vector
    const Vec2 b = p2 - p1;
    const double b2 = b.dot(b);
    if (b2 == 0.0) {
        return false; // degenerate segment
    }

    // Project to closest point on segment
    const Vec2 a = pos - p1;
    double t = a.dot(b) / b2;
    t = std::clamp(t, 0.0, 1.0);

    const Vec2 closest = p1 + b * t;
    const Vec2 n = pos - closest;
    const double dist2 = n.dot(n);
    const double r = this->radius;

    if (dist2 >= r * r) {
        return false; // no collision
    }

    // Calculate normal
    const double dist = std::sqrt(dist2);
    Vec2 nhat = (dist > 1e-12) ? n / dist : Vec2(0.0, 1.0);

    // Reflect velocity
    const double vn = velo.dot(nhat);
    velo = velo - nhat * (2.0 * vn);

    // Add gravity for the full time step
    pos = pos + velo * deltaTime + gravity * (0.5 * deltaTime * deltaTime);
    velo = velo + gravity * deltaTime;

    // Push ball out of collision
    const Vec2 new_a = pos - p1;
    double new_t = new_a.dot(b) / b2;
    new_t = std::clamp(new_t, 0.0, 1.0);
    
    const Vec2 new_closest = p1 + b * new_t;
    Vec2 new_n = pos - new_closest;
    const double new_dist2 = new_n.dot(new_n);
    
    if (new_dist2 < r * r) {
        const double new_dist = std::sqrt(new_dist2);
        Vec2 new_nhat = (new_dist > 1e-12) ? new_n / new_dist : Vec2(0.0, 1.0);
        const double penetration = r - new_dist;
        pos = pos + new_nhat * penetration;
    }

    return true;
}



