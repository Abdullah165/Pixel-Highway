#ifndef PIXEL_HIGHWAY_NPCCARCONTROLLER_H
#define PIXEL_HIGHWAY_NPCCARCONTROLLER_H

#include "raylib.h"


class NpcCarController
{
public:
    NpcCarController(Texture2D car, Vector2 position, float speed);
    ~NpcCarController();

    void Draw() const;
    void Update(Vector2 roadPosition, int roadWidth);
    void RePosition(Vector2 roadPosition, int roadWidth);
    void ChangeSpeedRandomly(int min, int max);

private:
    Texture2D m_texture;
    Vector2 m_position;
    float m_speed;
};


#endif //PIXEL_HIGHWAY_NPCCARCONTROLLER_H