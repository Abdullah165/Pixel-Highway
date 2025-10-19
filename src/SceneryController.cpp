#include "../include/SceneryController.h"

SceneryController::SceneryController() : m_texture(nullptr), m_position(Vector2{0, 0})
{
}

SceneryController::SceneryController(Texture2D* texture, Vector2 position) :
    m_texture(texture), m_position(position)
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
        ReSpawn();
    }
}

void SceneryController::ReSpawn()
{
    // RePosition.
    m_position.y = -10;
}

void SceneryController::SetPosition(const Vector2& position)
{
    m_position = position;
}
