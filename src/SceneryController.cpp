#include "../include/SceneryController.h"

SceneryController::SceneryController(Texture2D car, Vector2 position, float speed)
{
    m_texture = car;
    m_position = position;

    m_speed = speed;
}

SceneryController::~SceneryController()
{
    UnloadTexture(m_texture);
}

void SceneryController::Draw() const
{
    DrawTexture(m_texture, m_position.x, m_position.y, WHITE);
}

void SceneryController::Update()
{
    m_position.y += m_speed;

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
