#include "../include/NpcCarController.h"

#include <random>

NpcCarController::NpcCarController()
    : m_Texture(nullptr), m_Position{0, 0}
{
}

NpcCarController::NpcCarController(Texture2D* carTexture, Vector2 position)
    : m_Texture(carTexture), m_Position(position)
{
}


void NpcCarController::Draw() const
{
    DrawTexture(*m_Texture, m_Position.x, m_Position.y, WHITE);
}

void NpcCarController::Update(const Vector2& roadPosition, int roadWidth, float worldSpeed)
{
    m_Position.y += worldSpeed;

    // Check if the car gets the end of road.
    if (m_Position.y > GetScreenHeight() + 10.0)
    {
        Respawn(roadPosition, roadWidth);
    }
}

void NpcCarController::Respawn(const Vector2& roadPosition, int roadWidth)
{
    // RePosition.
    m_Position.y = -10;

    const int min_value = roadPosition.x + 20; // beginning of the road
    const int max_value = roadPosition.x + roadWidth - 70; // End of the road

    std::random_device rd; // non-deterministic random seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> dist(min_value, max_value);

    m_Position.x = dist(gen);
}

void NpcCarController::SetPosition(const Vector2& position)
{
    m_Position = position;
}

Rectangle NpcCarController::getRect() const
{
    return Rectangle(m_Position.x + 12, m_Position.y + 5, static_cast<float>(m_Texture->width - 25),
                     static_cast<float>(m_Texture->height - 15));
}

