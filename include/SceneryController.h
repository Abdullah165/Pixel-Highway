#ifndef PIXEL_HIGHWAY_SCENERYCONTROLLER_H
#define PIXEL_HIGHWAY_SCENERYCONTROLLER_H
#include "raylib.h"


class SceneryController
{
public:
    SceneryController();
    SceneryController(Texture2D* texture, Vector2 position);

    void Draw() const;
    void Update(float worldSpeed);
    void ReSpawn();
    void SetPosition(const Vector2& position);

private:
    Texture2D* m_texture;
    Vector2 m_position;
};


#endif //PIXEL_HIGHWAY_SCENERYCONTROLLER_H