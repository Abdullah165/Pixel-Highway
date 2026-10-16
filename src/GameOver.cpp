#include "../include/GameOver.h"

void GameOver::Init(float roadXPos, float roadWidth)
{
    m_roadX = roadXPos;
    m_roadWidth = roadWidth;

    // All buttons are centered on the road, in one column.
    const float buttonWidth = 220.0f;
    const float buttonHeight = 50.0f;
    const float x = roadXPos + (roadWidth - buttonWidth) / 2.0f;

    // Fixed positions: Play Again and Menu never move, even when an ad button is hidden.
    m_doubleButtonBounds = Rectangle{ x, 320.0f, buttonWidth, buttonHeight };
    m_continueButtonBounds = Rectangle{ x, 390.0f, buttonWidth, buttonHeight };
    m_playAgainButtonBounds = Rectangle{ x, 460.0f, buttonWidth, buttonHeight };
    m_menuButtonBounds = Rectangle{ x, 530.0f, buttonWidth, buttonHeight };

    m_doubleTextColor = GOLD;
    m_continueTextColor = RED;
    m_playAgainTextColor = RED;
    m_menuTextColor = RED;
}

void GameOver::Draw(const Font& font, bool showContinue, int doubleBonus) const
{
    DrawRectangle(static_cast<int>(m_roadX), 0, static_cast<int>(m_roadWidth),
        GetScreenHeight(), ColorAlpha(BLACK, 0.5f));

    const char* title = "Game Over";
    const Vector2 titleSize = MeasureTextEx(font, title, 35, 4);
    DrawTextEx(font, title, Vector2{ m_roadX + (m_roadWidth - titleSize.x) / 2.0f, 200.0f }, 35, 4, WHITE);

    if (doubleBonus > 0)
        DrawButton(font, m_doubleButtonBounds, TextFormat("+%i Coins (Ad)", doubleBonus), m_doubleTextColor);

    if (showContinue)
        DrawButton(font, m_continueButtonBounds, "Continue (Ad)", m_continueTextColor);

    DrawButton(font, m_playAgainButtonBounds, "Play Again", m_playAgainTextColor);
    DrawButton(font, m_menuButtonBounds, "Menu", m_menuTextColor);
}

void GameOver::DrawButton(const Font& font, const Rectangle& bounds, const char* text, Color textColor) const
{
    DrawRectangleRounded(bounds, 0.7f, 8, BLACK);

    const float fontSize = 22.0f;
    const float spacing = 3.0f;
    const Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(font, text,
        Vector2{ bounds.x + (bounds.width - size.x) / 2.0f, bounds.y + (bounds.height - size.y) / 2.0f },
        fontSize, spacing, textColor);
}

Rectangle GameOver::GetPlayAgainButtonRec() const
{
    return m_playAgainButtonBounds;
}

Rectangle GameOver::GetContinueButtonRec() const
{
    return m_continueButtonBounds;
}

Rectangle GameOver::GetMenuButtonRec() const
{
    return m_menuButtonBounds;
}

Rectangle GameOver::GetDoubleButtonRec() const
{
    return m_doubleButtonBounds;
}

void GameOver::ChangePlayAgainTextColor(Color newColor)
{
    m_playAgainTextColor = newColor;
}

void GameOver::ChangeContinueTextColor(Color newColor)
{
    m_continueTextColor = newColor;
}

void GameOver::ChangeMenuTextColor(Color newColor)
{
    m_menuTextColor = newColor;
}

void GameOver::ChangeDoubleTextColor(Color newColor)
{
    m_doubleTextColor = newColor;
}