#include "../include/NpcCarController.h"

NpcCarController::NpcCarController()
    : m_texture(nullptr), m_position{ 0, 0 }
{}

NpcCarController::NpcCarController(Texture2D* carTexture, Vector2 position)
    : m_texture(carTexture), m_position(position)
{}


void NpcCarController::Draw() const
{
    DrawTexture(*m_texture, static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
}

// worldSpeed here is the distance to move this frame (already multiplied by dt).
void NpcCarController::Update(const Vector2& roadPosition, int roadWidth, float worldSpeed)
{
    m_position.y += worldSpeed;

    // Check if the car reached the end of the road.
    if (m_position.y > GetScreenHeight() + 10.0f)
    {
        Respawn(roadPosition, roadWidth);
    }
}

void NpcCarController::Respawn(const Vector2& roadPosition, int roadWidth)
{
    // Back above the screen
    m_position.y = -static_cast<float>(m_texture->height);

    const int minX = static_cast<int>(roadPosition.x) + 20;            // beginning of the road
    const int maxX = static_cast<int>(roadPosition.x) + roadWidth - 70; // end of the road

    m_position.x = static_cast<float>(GetRandomValue(minX, maxX));

    ResetNearMiss(); // a new pass starts
}

void NpcCarController::SetPosition(const Vector2& position)
{
    m_position = position;
    ResetNearMiss();
}

Rectangle NpcCarController::getRect() const
{
    return Rectangle{ m_position.x + 12, m_position.y + 5,
                     static_cast<float>(m_texture->width - 20),
                     static_cast<float>(m_texture->height - 15) };
}

bool NpcCarController::WasNear() const
{
    return m_wasNear;
}

void NpcCarController::SetWasNear(bool value)
{
    m_wasNear = value;
}

bool NpcCarController::IsNearMissHandled() const
{
    return m_nearMissHandled;
}

void NpcCarController::MarkNearMissHandled()
{
    m_nearMissHandled = true;
}

void NpcCarController::ResetNearMiss()
{
    m_wasNear = false;
    m_nearMissHandled = false;
}