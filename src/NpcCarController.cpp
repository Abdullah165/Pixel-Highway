#include "../include/NpcCarController.h"

#include <iostream>
#include <random>

NpcCarController::NpcCarController(Texture2D car, Vector2 position, float speed)
{
    m_texture = car;
    m_position = position;

    m_speed = speed;
}

NpcCarController::~NpcCarController()
{
    UnloadTexture(m_texture);
}

void NpcCarController::Draw() const
{
    DrawTexture(m_texture, m_position.x, m_position.y, WHITE);
}

void NpcCarController::Update(Vector2 roadPosition, int roadWidth)
{
    m_position.y += m_speed;

    // Check if the car gets the end of road.
    if (m_position.y > GetScreenHeight() + 10.0)
    {
        RePosition(roadPosition, roadWidth);
        //ChangeSpeedRandomly(5, 6);
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
    std::cout << "Random: " <<  dist(gen) << "\n";
}

void NpcCarController::ChangeSpeedRandomly(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);

     m_speed = dist(gen);
}
