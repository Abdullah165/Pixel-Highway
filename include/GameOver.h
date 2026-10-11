#ifndef PIXEL_HIGHWAY_GAMEOVER_H
#define PIXEL_HIGHWAY_GAMEOVER_H
#include "raylib.h"

class GameOver
{
public:
    void Init(float roadXPos, float roadWidth);
    void Draw(const Font& font, bool showContinue) const;

    Rectangle GetPlayAgainButtonRec() const;
    Rectangle GetContinueButtonRec() const;
    void ChangePlayAgainTextColor(Color newColor);
    void ChangeContinueTextColor(Color newColor);

private:
    void DrawButton(const Font& font, const Rectangle& bounds, const char* text, Color textColor) const;

    float m_roadX = 0.0f;
    float m_roadWidth = 0.0f;

    Rectangle m_playAgainButtonBounds{};
    Rectangle m_continueButtonBounds{};
    Color m_playAgainTextColor = RED;
    Color m_continueTextColor = RED;
};


#endif //PIXEL_HIGHWAY_GAMEOVER_H