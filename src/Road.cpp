//
// Created by Lenovo on 9/21/2025.
//

#include "../include/Road.h"

#include <iostream>

Road::Road()
{

}

Road::~Road()
{

}

void Road::InitResources()
{
    for (int i = 0; i < m_texture.size(); i++)
    {
        m_texture[i] = LoadTexture("assets/textures/road/road_64px_a_3.png");
    }

    m_position = Vector2(GetScreenWidth() / 2.5f, 0);
}
void Road::UnloadResources()
{
    for (int i = 0; i < m_texture.size(); i++)
    {
        UnloadTexture(m_texture[i]);
    }
}

void Road::Draw() const
{
    DrawTexture(m_texture[0], m_position.x, m_position.y,WHITE);
    for (int i = 1; i < m_texture.size(); i++)
    {
        DrawTexture(m_texture[i], m_position.x, m_texture[i].height * i,WHITE);
    }
}

Vector2 Road::GetPosition() const
{
    return m_position;
}

int Road::GetWidth() const
{
    return m_texture[0].width;
}