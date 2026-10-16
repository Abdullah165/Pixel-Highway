#ifndef PIXEL_HIGHWAY_STEERING_H
#define PIXEL_HIGHWAY_STEERING_H

#include <algorithm>

// How fast the player's car steers, depending on how fast the world is moving.
// Used by the car and by the coin patterns, so both always agree.
namespace Steering
{
    constexpr float BASE_SPEED = 3.0f;       // steering speed at BASE_WORLD_SPEED
    constexpr float BASE_WORLD_SPEED = 3.0f; // the Easy starting speed
    constexpr float GROWTH = 0.45f;           // extra steering speed per +1 world speed

    inline float SpeedFor(float worldSpeed)
    {
        return BASE_SPEED + std::max(0.0f, worldSpeed - BASE_WORLD_SPEED) * GROWTH;
    }
}

#endif //PIXEL_HIGHWAY_STEERING_H