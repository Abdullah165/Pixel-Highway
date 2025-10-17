#ifndef PIXEL_HIGHWAY_GAMEOVER_H
#define PIXEL_HIGHWAY_GAMEOVER_H
#include "raylib.h"


class GameOver
{
public:
    void InitResources();

    void Draw(float roadXPos, float roadWidth,Font& font) const;

private:
    bool m_isGameOver;
};


#endif //PIXEL_HIGHWAY_GAMEOVER_H
