#include "../include/CarController.h"

#include <format>
#include <iostream>
#include <ostream>
#include <string>

CarController::CarController()
{
}


void CarController::InitResources()
{
    if (m_loaded) return;

    m_texture[0] = LoadTexture("assets/textures/car/car_24px_8way_white_6.png");
    m_texture[1] = LoadTexture("assets/textures/car/car_24px_8way_white_7.png");
    m_texture[2] = LoadTexture("assets/textures/car/car_24px_8way_white_8.png");

    m_position = Vector2{GetScreenWidth() / 2.2f, GetScreenHeight() - 150.0f};

    for (int i = 0; i < m_explosionTextures.size(); i++)
    {
        std::string filePath = std::format("assets/textures/explosion/frames/1_{}.png", i);
        m_explosionTextures[i] = LoadTexture(filePath.c_str());
    }

    m_loaded = true;
}

void CarController::UnloadResources()
{
    if (!m_loaded) return;

    UnloadTexture(m_texture[0]);
    UnloadTexture(m_texture[1]);
    UnloadTexture(m_texture[2]);

    for (const auto& m_explosionTexture : m_explosionTextures)
    {
        UnloadTexture(m_explosionTexture);
    }

    m_loaded = false;
}

void CarController::Draw()
{
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        DrawTexture(m_texture[2], m_position.x, m_position.y, WHITE);
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        DrawTexture(m_texture[0], m_position.x, m_position.y, WHITE);
    else
        DrawTexture(m_texture[1], m_position.x, m_position.y, WHITE);

    if (m_isExploding)
    {
        m_explosionTimer += GetFrameTime();

        if (m_explosionTimer >= 0.1f)
        {
            m_explosionFrame++;
            m_explosionTimer = 0.0f;
        }

        if (m_explosionFrame < m_explosionTextures.size())
        {
            DrawTexture(m_explosionTextures[m_explosionFrame], m_position.x - 30, m_position.y - 70, WHITE);
        }
        else
        {
            m_isExploding = false;
        }
    }
}

void CarController::Update(const Vector2& roadPosition, int roadWidth)
{
    if (IsKeyDown(KEY_D)|| IsKeyDown(KEY_RIGHT))
    {
        if (m_position.x < roadPosition.x + roadWidth - 70)
            m_position.x += m_speed;
    }
    else if (IsKeyDown(KEY_A)|| IsKeyDown(KEY_LEFT))
    {
        if (m_position.x > roadPosition.x + 20)
            m_position.x -= m_speed;
    }
}

Rectangle CarController::getRect() const
{
    return Rectangle(m_position.x + 12, m_position.y + 5, static_cast<float>(m_texture[0].width - 25),
                     static_cast<float>(m_texture[0].height - 15));
}

void CarController::PlayCarExplosionVFX()
{
    m_isExploding = true;
    m_explosionFrame = 0;
    m_explosionTimer = 0.0f;
}
