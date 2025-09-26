#include <vector>

#include "raylib.h"
#include "../include/CarController.h"
#include "../include/NpcCarController.h"
#include "../include/Road.h"

int main()
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Pixel Highway Example");

    SetTargetFPS(60);

    CarController car_controller;

    Road road;

    std::vector<NpcCarController> npc_cars;
    npc_cars.reserve(4);

    Texture2D blue_car = LoadTexture("assets/textures/car/car_24px_blue_2.png");
    Texture2D green_car = LoadTexture("assets/textures/car/car_24px_green_4.png");
    Texture2D yellow_car = LoadTexture("assets/textures/car/car_24px_yellow_2.png");

    npc_cars.emplace_back(blue_car,Vector2{GetScreenWidth() / 1.9f, -50.0f},4.0);
    npc_cars.emplace_back(green_car,Vector2{GetScreenWidth() / 2.0f, -50.0f},4.0);
    npc_cars.emplace_back(yellow_car,Vector2{GetScreenWidth() / 2.4f, -50.0f},4.0);

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        car_controller.Update(road.GetPosition(), road.GetWidth());
        for (auto& npc_car_array : npc_cars)
        {
            npc_car_array.Update(road.GetPosition(), road.GetWidth());
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        road.Draw();

        car_controller.Draw();

        for (const auto& npc_car_array : npc_cars)
        {
            npc_car_array.Draw();
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------
    return 0;
}
