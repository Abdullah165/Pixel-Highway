#ifndef PIXEL_HIGHWAY_CARCONTROLLER_H
#define PIXEL_HIGHWAY_CARCONTROLLER_H
#include <array>

#include "raylib.h"


class CarController
{
public:
    CarController();

    void InitResources();
    void UnloadResources();

    void Draw();
    void Update(const Vector2& roadPosition, int roadWidth);
    Rectangle getRect() const;

    void PlayCarExplosionVFX();

private:
    std::array<Texture2D, 3> m_texture;
    std::array<Texture2D, 33> m_explosionTextures;
    Vector2 m_position{};
    float m_speed = 3.0f;
    bool m_loaded = false;
    bool m_isExploding = false;
    int m_explosionFrame = 0;
    float m_explosionTimer = 0.0f;
};


#endif //PIXEL_HIGHWAY_CARCONTROLLER_H
