#include "../include/Ball.hpp"

std::unordered_map<int, SDL_Texture*> Ball::textureCache;

Ball::Ball(const Vec2& pos, const Vec2& velo, int radius, SDL_Renderer* renderer)
    : pos(pos), velo(velo), radius(radius), texture(nullptr)
{
    int diameter = radius * 2;
    int texSize = diameter + 1; // padding for outermost pixels

    auto it = textureCache.find(radius);
    if (it != textureCache.end()) {
        texture = it->second;
        return;
    }

    // Create 32-bit RGBA surface
    SDL_Surface* surface = SDL_CreateRGBSurface(
        0, texSize, texSize, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#else
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#endif
    );

    if (!surface) {
        SDL_Log("SDL_CreateRGBSurface failed: %s", SDL_GetError());
        return;
    }

    // Fill surface with transparent pixels
    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

    // Lock and get pixels
    SDL_LockSurface(surface);
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    Uint32 color = SDL_MapRGBA(surface->format, 214, 23, 57, 255); // outline color

    // helper to safely set a pixel inside bounds
    auto setPixel = [&](int x, int y) {
        if (x >= 0 && x < texSize && y >= 0 && y < texSize) {
            pixels[y * texSize + x] = color;
        }
    };

    // --- midpoint circle (hollow) ---
    int cx = radius;
    int cy = radius;
    int r = radius;

    int dx = 0;
    int dy = r;
    int d = 1 - r;

    while (dx <= dy)
    {
        setPixel(cx + dx, cy + dy);
        setPixel(cx + dy, cy + dx);
        setPixel(cx - dx, cy + dy);
        setPixel(cx - dy, cy + dx);
        setPixel(cx + dx, cy - dy);
        setPixel(cx + dy, cy - dx);
        setPixel(cx - dx, cy - dy);
        setPixel(cx - dy, cy - dx);

        if (d < 0) {
            d += 2 * dx + 3;
        } else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
    }
    // --------------------------------

    SDL_UnlockSurface(surface);

    // Create texture from surface and enable alpha blending
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textureCache[radius] = texture;
    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
    } else {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    SDL_FreeSurface(surface);
}

// Render: single fast copy
void Ball::renderBall(SDL_Renderer* renderer) const {
    SDL_Texture* tex = textureCache[radius];
    if (!tex) return;
    SDL_Rect dst;
    dst.x = static_cast<int>(pos.getX()) - radius;
    dst.y = static_cast<int>(pos.getY()) - radius;
    dst.w = radius * 2;
    dst.h = radius * 2;
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
}

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

Vec2 Ball::getRandomPosVector(int WIDTH, int HEIGHT, int ballRad, int grinderCircleRad) {
    static std::mt19937 rng(
        static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    );

    int half_height = HEIGHT / 2;
    int minY = ballRad;
    int maxY = half_height - grinderCircleRad - ballRad;

    std::uniform_int_distribution<int> distX(ballRad, WIDTH - ballRad);
    std::uniform_int_distribution<int> distY(minY, maxY);

    int randomX;


    randomX = distX(rng);
    int randomY = distY(rng);
    //std::cout << randomX << ", " << randomY << std::endl;

    return Vec2(randomX, randomY);
}


