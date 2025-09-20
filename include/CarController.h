//
// Created by Lenovo on 9/20/2025.
//

#ifndef PIXEL_HIGHWAY_CARCONTROLLER_H
#define PIXEL_HIGHWAY_CARCONTROLLER_H
#include <array>

#include "raylib.h"


class CarController
{
public:
    CarController();
    ~CarController();

    void Draw() const;
    void Update();

private:
    std::array<Texture2D,3> texture;
    Vector2 position;

    float speed;
};


#endif //PIXEL_HIGHWAY_CARCONTROLLER_H