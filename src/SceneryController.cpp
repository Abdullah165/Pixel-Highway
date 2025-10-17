#include "../include/SceneryController.h"

SceneryController::SceneryController() : m_texture(nullptr), m_position(Vector2{0, 0}), m_speed(0)
{
}

SceneryController::SceneryController(Texture2D* texture, Vector2 position, float speed) :
    m_texture(texture), m_position(position), m_speed(speed)
{
}

void SceneryController::Draw() const
{
    DrawTexture(*m_texture, m_position.x, m_position.y, WHITE);
}

void SceneryController::Update(float worldSpeed)
{
    m_position.y += worldSpeed;

    // Check if the scenery gets the end of road.
    if (m_position.y > GetScreenHeight() + 10.0)
    {
        RePosition();
    }
}

void SceneryController::RePosition()
{
    // RePosition.
    m_position.y = -10;
}
