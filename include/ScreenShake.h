#ifndef PIXEL_HIGHWAY_SCREENSHAKE_H
#define PIXEL_HIGHWAY_SCREENSHAKE_H

#include "raylib.h"

// Random camera offset that starts strong and fades to zero.
class ScreenShake
{
public:
    void Start(float duration, float magnitude)
    {
        m_duration = duration;
        m_timeLeft = duration;
        m_magnitude = magnitude;
    }

    void Stop()
    {
        m_timeLeft = 0.0f;
    }

    void Update(float deltaSeconds)
    {
        if (m_timeLeft > 0.0f)
        {
            m_timeLeft -= deltaSeconds;
            if (m_timeLeft < 0.0f) m_timeLeft = 0.0f;
        }
    }

    // How many pixels the whole world should move this frame.
    Vector2 GetOffset() const
    {
        if (m_timeLeft <= 0.0f || m_duration <= 0.0f)
            return Vector2{ 0.0f, 0.0f };

        const float strength = m_magnitude * (m_timeLeft / m_duration); // fades out

        return Vector2{
            (GetRandomValue(-100, 100) / 100.0f) * strength,
            (GetRandomValue(-100, 100) / 100.0f) * strength
        };
    }

private:
    float m_duration = 0.0f;
    float m_timeLeft = 0.0f;
    float m_magnitude = 0.0f;
};

#endif //PIXEL_HIGHWAY_SCREENSHAKE_H