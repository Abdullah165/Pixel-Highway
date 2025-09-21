//
// Created by Lenovo on 9/21/2025.
//

#include "../include/Road.h"

#include <iostream>

Road::Road()
{
    for (int i = 0; i < texture.size(); i++)
    {
        texture[i] = LoadTexture("assets/textures/road/road_64px_a_3.png");
    }
    position = Vector2(GetScreenWidth() / 2.5f, 0);
}

Road::~Road()
{
    for (int i = 0; i < texture.size(); i++)
    {
        UnloadTexture(texture[i]);
    }
}

void Road::Draw() const
{
    DrawTexture(texture[0], position.x, position.y,WHITE);
    for (int i = 1; i < texture.size(); i++)
    {
        DrawTexture(texture[i], position.x, texture[i].height * i,WHITE);
    }
}
