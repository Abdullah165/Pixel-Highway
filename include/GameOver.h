#ifndef PIXEL_HIGHWAY_GAMEOVER_H
#define PIXEL_HIGHWAY_GAMEOVER_H
#include "raylib.h"

class GameOver
{
public:
    void Init(float);
    void Draw(float roadXPos, float roadWidth,Font& font) const;
    Rectangle GetPlayAgainButtonRec() const;
    void ChangePlayAgainTextColor(Color);
private:
    Rectangle m_playAgainButtonBounds;
    Color m_playAgainTextColor;
};


#endif //PIXEL_HIGHWAY_GAMEOVER_H
