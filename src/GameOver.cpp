#include "../include/GameOver.h"

#include "raylib.h"


void GameOver::InitResources()
{
    m_font = LoadFont("assets/fonts/JungleAdventurer.otf");
}

void GameOver::Draw(float roadXPos, float roadWidth) const
{
    DrawRectangle(roadXPos, 0, roadWidth, GetScreenHeight(), ColorAlpha(BLACK, 0.5f));

    DrawTextEx(m_font,"Game Over",(Vector2){ roadXPos * 1.07f, 200.0f },35,4,WHITE);

    DrawTextEx(m_font,"Play Again",(Vector2){ roadXPos * 1.07f, 400.0f },30,5,WHITE);
}
