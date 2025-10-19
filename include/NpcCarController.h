#ifndef PIXEL_HIGHWAY_NPCCARCONTROLLER_H
#define PIXEL_HIGHWAY_NPCCARCONTROLLER_H

#include "raylib.h"


class NpcCarController
{
public:
    NpcCarController();
    NpcCarController(Texture2D* carTexture, Vector2 position);

    void Draw() const;
    void Update(const Vector2& roadPosition, int roadWidth, float worldSpeed);
    void Respawn(const Vector2& roadPosition, int roadWidth);
    Rectangle getRect() const;
    void SetPosition(const Vector2& position);
private:
    Texture2D* m_Texture;
    Vector2 m_Position;
};


#endif //PIXEL_HIGHWAY_NPCCARCONTROLLER_H