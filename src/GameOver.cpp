#include "../include/GameOver.h"

#include "raylib.h"

void GameOver::Init(float roadXPos)
{
     m_playAgainButtonBounds = (Rectangle{roadXPos * 1.06f, 390, 200, 50});

    m_playAgainTextColor = RED;
}


void GameOver::Draw(float roadXPos, float roadWidth, Font& font) const
{
    DrawRectangle(roadXPos, 0, roadWidth, GetScreenHeight(), ColorAlpha(BLACK, 0.5f));

    DrawTextEx(font, "Game Over", (Vector2){roadXPos * 1.07f, 200.0f}, 35, 4,WHITE);

    DrawRectangleRounded(Rectangle{roadXPos * 1.06f, 390, 200, 50}, 0.7f, 1,BLACK);
    DrawTextEx(font, "Play Again", (Vector2){roadXPos * 1.1f, 405.0f}, 25, 5,m_playAgainTextColor);
}

Rectangle GameOver::GetPlayAgainButtonRec() const
{
    return  m_playAgainButtonBounds;
}

void GameOver::ChangePlayAgainTextColor(Color newColor)
{
    m_playAgainTextColor = newColor;
}
