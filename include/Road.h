//
// Created by Lenovo on 9/21/2025.
//

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

private:
    std::array<Texture2D,3> texture;
    Vector2 position;
};


#endif //PIXEL_HIGHWAY_ROAD_H
