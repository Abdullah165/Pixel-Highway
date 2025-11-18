#include "../include/NpcCarController.h"

#include <random>

NpcCarController::NpcCarController()
    : m_texture(nullptr), m_position{0, 0}
{
}

NpcCarController::NpcCarController(Texture2D* carTexture, Vector2 position)
    : m_texture(carTexture), m_position(position)
{
}


void NpcCarController::Draw() const
{
    DrawTexture(*m_texture, m_position.x, m_position.y, WHITE);
}

void NpcCarController::Update(const Vector2& roadPosition, int roadWidth, float worldSpeed)
{
    m_position.y += worldSpeed;

    // Check if the car gets the end of road.
    if (m_position.y > GetScreenHeight() + 10.0)
    {
        Respawn(roadPosition, roadWidth);
    }
}

void NpcCarController::Respawn(const Vector2& roadPosition, int roadWidth)
{
    // RePosition.
    m_position.y = 0.0f - m_texture->height;

    const int min_value = roadPosition.x + 20; // beginning of the road
    const int max_value = roadPosition.x + roadWidth - 70; // End of the road

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min_value, max_value);

    m_position.x = dist(gen);
}

void NpcCarController::SetPosition(const Vector2& position)
{
    m_position = position;
}

Rectangle NpcCarController::getRect() const
{
    return Rectangle(m_position.x + 12, m_position.y + 5, static_cast<float>(m_texture->width - 25),
                     static_cast<float>(m_texture->height - 15));
}

