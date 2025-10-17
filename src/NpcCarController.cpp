#include "../include/NpcCarController.h"

#include <random>

NpcCarController::NpcCarController()
    : m_texture(nullptr), m_position{0, 0}, m_init_speed(0.0f)
{}

NpcCarController::NpcCarController(Texture2D* carTexture, Vector2 position, float speed)
    : m_texture(carTexture), m_position(position), m_init_speed(speed)
{}

void NpcCarController::Draw() const
{
    DrawTexture(*m_texture, m_position.x, m_position.y, WHITE);
}

void NpcCarController::Update(Vector2 roadPosition, int roadWidth,float worldSpeed)
{
    m_position.y += worldSpeed;

    // Check if the car gets the end of road.
    if (m_position.y > GetScreenHeight() + 10.0)
    {
        RePosition(roadPosition, roadWidth);
    }
}

void NpcCarController::RePosition(Vector2 roadPosition, int roadWidth)
{
    // RePosition.
    m_position.y = -10;

    const int min_value = roadPosition.x + 20; // beginning of the road
    const int max_value = roadPosition.x + roadWidth - 70; // End of the road

    std::random_device rd; // non-deterministic random seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> dist(min_value, max_value);

    m_position.x = dist(gen);
}

Rectangle NpcCarController::getRect() const
{
    return Rectangle(m_position.x + 12, m_position.y + 5, static_cast<float>(m_texture->width - 25),
                     static_cast<float>(m_texture->height - 15));
}
