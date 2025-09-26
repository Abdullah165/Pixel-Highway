//
// Created by Lenovo on 9/20/2025.
//

#include "../include/CarController.h"

CarController::CarController()
{
    m_texture[0] = LoadTexture("assets/textures/car/car_24px_8way_white_6.png");
    m_texture[1] = LoadTexture("assets/textures/car/car_24px_8way_white_7.png");
    m_texture[2] = LoadTexture("assets/textures/car/car_24px_8way_white_8.png");

    m_position = Vector2{GetScreenWidth() / 2.2f, GetScreenHeight() - 150.0f};
    m_speed = 3.0f;
}

CarController::~CarController()
{
    UnloadTexture(m_texture[0]);
    UnloadTexture(m_texture[1]);
    UnloadTexture(m_texture[2]);
}


void CarController::Draw() const
{
    if (IsKeyDown(KEY_D))
    {
        DrawTexture(m_texture[2], m_position.x, m_position.y,WHITE);
    }
    else if (IsKeyDown(KEY_A))
    {
        DrawTexture(m_texture[0], m_position.x, m_position.y,WHITE);
    }
    else
    {
        DrawTexture(m_texture[1], m_position.x, m_position.y,WHITE);
    }
}

void CarController::Update(Vector2 roadPosition, int roadWidth)
{
    if (IsKeyDown(KEY_D))
    {
        if (m_position.x < roadPosition.x + roadWidth - 70)
            m_position.x += m_speed;
    }
    else if (IsKeyDown(KEY_A))
    {
        if (m_position.x > roadPosition.x + 20)
            m_position.x -= m_speed;
    }
}
