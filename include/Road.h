#ifndef PIXEL_HIGHWAY_ROAD_H
#define PIXEL_HIGHWAY_ROAD_H

#include "raylib.h"


class Road
{
public:
    void InitResources();
    void UnloadResources();

    void Draw() const;

    Vector2 GetPosition() const;
    int GetWidth() const;

private:
    static constexpr int TILE_COUNT = 3; // how many road images are stacked vertically

    Texture2D m_texture{};
    Vector2 m_position{};
};


#endif //PIXEL_HIGHWAY_ROAD_H