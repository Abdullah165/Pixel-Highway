#ifndef PIXEL_HIGHWAY_ROAD_H
#define PIXEL_HIGHWAY_ROAD_H
#include <array>

#include "raylib.h"


class Road
{
public:
    Road();
    ~Road();
    void Draw() const;

    Vector2 GetPosition() const;
    int GetWidth() const;

private:
    std::array<Texture2D,3> m_texture;
    Vector2 m_position;
};


#endif //PIXEL_HIGHWAY_ROAD_H
