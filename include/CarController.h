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
    void Update(Vector2 roadPosition, int roadWidth);
    Rectangle getRect() const;
private:
    std::array<Texture2D,3> m_texture;
    Vector2 m_position;
    float m_speed;
};


#endif //PIXEL_HIGHWAY_CARCONTROLLER_H