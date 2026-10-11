#include "../include/Road.h"

void Road::InitResources()
{
    m_texture = LoadTexture("assets/textures/road/road_64px_a_3.png");
    m_position = Vector2{GetScreenWidth() / 2.5f, 0.0f};
}

void Road::UnloadResources()
{
    UnloadTexture(m_texture);
}

void Road::Draw() const
{
    for (int i = 0; i < TILE_COUNT; i++)
    {
        DrawTexture(m_texture,
                    static_cast<int>(m_position.x),
                    static_cast<int>(m_position.y) + m_texture.height * i,
                    WHITE);
    }
}

Vector2 Road::GetPosition() const
{
    return m_position;
}

int Road::GetWidth() const
{
    return m_texture.width;
}