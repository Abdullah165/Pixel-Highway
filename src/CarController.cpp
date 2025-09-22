//
// Created by Lenovo on 9/20/2025.
//

#include "../include/CarController.h"

CarController::CarController()
{
    texture[0] = LoadTexture("assets/textures/car/car_24px_8way_white_6.png");
    texture[1] = LoadTexture("assets/textures/car/car_24px_8way_white_7.png");
    texture[2] = LoadTexture("assets/textures/car/car_24px_8way_white_8.png");

    position = Vector2{GetScreenWidth() / 2.2f, GetScreenHeight() - 100.0f};
    speed = 3.0f;
}

CarController::~CarController()
{
    UnloadTexture(texture[0]);
    UnloadTexture(texture[1]);
    UnloadTexture(texture[2]);
}


void CarController::Draw() const
{
    if (IsKeyDown(KEY_D))
    {
        DrawTexture(texture[2], position.x, position.y,WHITE);
    }
    else if (IsKeyDown(KEY_A))
    {
        DrawTexture(texture[0], position.x, position.y,WHITE);
    }
    else
    {
        DrawTexture(texture[1], position.x, position.y,WHITE);
    }
}

void CarController::Update()
{
    if (IsKeyDown(KEY_D))
    {
        if (position.x < static_cast<float>(GetScreenWidth()) * 0.54f)
            position.x += speed;
    }
    else if (IsKeyDown(KEY_A))
    {
        if (position.x > static_cast<float>(GetScreenWidth()) * 0.42f)
            position.x -= speed;
    }
}
