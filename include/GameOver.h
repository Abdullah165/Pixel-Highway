#ifndef PIXEL_HIGHWAY_GAMEOVER_H
#define PIXEL_HIGHWAY_GAMEOVER_H
#include "raylib.h"

class GameOver
{
public:
    void Init(float roadXPos, float roadWidth);

    // doubleBonus > 0 shows the "+N Coins (Ad)" button.
    void Draw(const Font& font, bool showContinue, int doubleBonus) const;

    Rectangle GetPlayAgainButtonRec() const;
    Rectangle GetContinueButtonRec() const;
    Rectangle GetMenuButtonRec() const;
    Rectangle GetDoubleButtonRec() const;

    void ChangePlayAgainTextColor(Color newColor);
    void ChangeContinueTextColor(Color newColor);
    void ChangeMenuTextColor(Color newColor);
    void ChangeDoubleTextColor(Color newColor);

private:
    void DrawButton(const Font& font, const Rectangle& bounds, const char* text, Color textColor) const;

    float m_roadX = 0.0f;
    float m_roadWidth = 0.0f;

    Rectangle m_doubleButtonBounds{};
    Rectangle m_continueButtonBounds{};
    Rectangle m_playAgainButtonBounds{};
    Rectangle m_menuButtonBounds{};

    Color m_doubleTextColor = GOLD;
    Color m_continueTextColor = RED;
    Color m_playAgainTextColor = RED;
    Color m_menuTextColor = RED;
};


#endif //PIXEL_HIGHWAY_GAMEOVER_H