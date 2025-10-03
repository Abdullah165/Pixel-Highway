#ifndef PIXEL_HIGHWAY_SCENERYCONTROLLER_H
#define PIXEL_HIGHWAY_SCENERYCONTROLLER_H
#include "raylib.h"


class SceneryController
{
public:
    SceneryController(Texture2D texture, Vector2 position, float speed);
    ~SceneryController();

    void Draw() const;
    void Update();
    void RePosition();

private:
    Texture2D m_texture;
    Vector2 m_position;
    float m_speed;
};


#endif //PIXEL_HIGHWAY_SCENERYCONTROLLER_H