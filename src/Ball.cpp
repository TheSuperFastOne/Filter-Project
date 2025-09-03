#include "../include/Ball.hpp"

inline double clamp(double x, double minVal, double maxVal)
{
    return (x < minVal) ? minVal : (x > maxVal ? maxVal : x);
}

Ball::Ball(const Vec2& pos, const Vec2& velo, float radius, SDL_Renderer* renderer) : pos(pos), velo(velo), radius(radius), prevPos(Vec2(0,0)), prevVelo(Vec2(0,0))
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

void Ball::renderBallAt(SDL_Renderer* renderer, const Vec2& worldPos) const
{
    // Midpoint Circle Algorithm
    Vec2 centerPos(worldPos.getX() * 100, 1200 - worldPos.getY() * 100);
    int radiusPixels = static_cast<int>(radius * 100);

    int cx = centerPos.getX();
    int cy = centerPos.getY();
    int r  = radiusPixels;

    int dx = 0;
    int dy = r;
    int d  = 1 - r;

    SDL_SetRenderDrawColor(renderer, 218, 31, 15, 255);

    while (dx <= dy)
    {
        SDL_RenderDrawPoint(renderer, cx + dy, cy + dx);
        SDL_RenderDrawPoint(renderer, cx - dx, cy + dy);
        SDL_RenderDrawPoint(renderer, cx - dy, cy - dx);
        SDL_RenderDrawPoint(renderer, cx + dx, cy - dy);
        SDL_RenderDrawPoint(renderer, cx + dy, cy - dx);
        SDL_RenderDrawPoint(renderer, cx - dx, cy - dy);
        SDL_RenderDrawPoint(renderer, cx - dy, cy + dx);
        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);

        if (d < 0) {
            d += 2 * dx + 3;
        } else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
    }
} // This is kinda just copy paste from Ball::renderBall but like whatever


bool Ball::handleCollisionWithLineSegment(const Vec2& p1,
                                          const Vec2& p2,
                                          double deltaTime,
                                          const Vec2& gravity)
{
    const Vec2 b = p2 - p1;
    const double b2 = b.dot(b);
    if (b2 == 0.0) {
        return false;
    }

    
    Vec2 pos_pred = pos + velo * deltaTime + gravity * (0.5 * deltaTime * deltaTime);
    Vec2 vel_pred = velo + gravity * deltaTime;

    
    Vec2 a0 = pos - p1;
    double t0 = std::clamp(a0.dot(b) / b2, 0.0, 1.0);
    Vec2 closest0 = p1 + b * t0;
    Vec2 n0 = pos - closest0;
    double dist0 = n0.magnitude();

    // Closest point at end
    Vec2 a1 = pos_pred - p1;
    double t1 = std::clamp(a1.dot(b) / b2, 0.0, 1.0);
    Vec2 closest1 = p1 + b * t1;
    Vec2 n1 = pos_pred - closest1;
    double dist1 = n1.magnitude();

    const double r = this->radius;

    // If both outside, no collision
    if (dist0 >= r && dist1 >= r) 
    {
        return false;
    }

    // binary search
    double lo = 0.0, hi = deltaTime;
    for (int i = 0; i < 20; ++i) {
        double mid = 0.5 * (lo + hi);
        Vec2 pos_mid = pos + velo * mid + gravity * (0.5 * mid * mid);
        Vec2 a_mid = pos_mid - p1;
        double t_mid = std::clamp(a_mid.dot(b) / b2, 0.0, 1.0);
        Vec2 closest_mid = p1 + b * t_mid;
        Vec2 n_mid = pos_mid - closest_mid;
        double dist_mid = n_mid.magnitude();
        if (dist_mid > r) {
            lo = mid; // not yet collided
        } else {
            hi = mid; // penetrating, step back
        }
    }

    double toi = hi; // time of impact 


    pos = pos + velo * toi + gravity* (0.5 * toi * toi);
    velo = velo + gravity * toi;

    Vec2 a_hit = pos - p1;
    double t_hit = std::clamp(a_hit.dot(b) / b2, 0.0, 1.0);
    Vec2 closest_hit = p1 + b * t_hit;
    Vec2 n_hit = pos - closest_hit;
    double d_hit = n_hit.magnitude();
    Vec2 nhat = (d_hit > 1e-12) ? n_hit / d_hit : Vec2(0.0, 1.0);

    // Reflect velocity with normalized n i might change this later to work with unnormalized but like I couldn't be bothered
    double vn = velo.dot(nhat);
    if (vn < 0.0) {
        velo = velo - nhat * (2.0 * vn);
    }

    // okay this should be exact you know
    double remain = deltaTime - toi;
    pos = pos + velo * remain + gravity * (0.5 * remain * remain);
    velo = velo + gravity * remain;

    return true;
}



