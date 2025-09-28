#include <vector>

#include "raylib.h"
#include "../include/CarController.h"
#include "../include/NpcCarController.h"
#include "../include/Road.h"
#include "../include/SceneryController.h"

int main()
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Pixel Highway Example");

    SetTargetFPS(60);

    CarController car_controller;

    Road road;

    //Npc cars
    std::vector<NpcCarController> npc_cars;
    npc_cars.reserve(6);

    Texture2D blue_car = LoadTexture("assets/textures/car/blue_car.png");
    Texture2D green_car = LoadTexture("assets/textures/car/green_car.png");
    Texture2D yellow_car = LoadTexture("assets/textures/car/yellow_car.png");
    Texture2D pink_car = LoadTexture("assets/textures/car/pink_car.png");
    Texture2D grey_car = LoadTexture("assets/textures/car/grey_car.png");
    Texture2D wreck_car = LoadTexture("assets/textures/car/wreck_car.png");

    npc_cars.emplace_back(blue_car, Vector2{GetScreenWidth() / 1.85f, -50.0f}, 5.0);
    npc_cars.emplace_back(green_car, Vector2{GetScreenWidth() / 2.0f, -130.0f}, 5.0);
    npc_cars.emplace_back(yellow_car, Vector2{GetScreenWidth() / 2.4f, -250.0f}, 5.0);
    npc_cars.emplace_back(pink_car, Vector2{GetScreenWidth() / 2.25f, -400.0f}, 5.0);
    npc_cars.emplace_back(grey_car, Vector2{GetScreenWidth() / 2.25f, -500.0f}, 5.0);
    npc_cars.emplace_back(wreck_car, Vector2{GetScreenWidth() / 2.25f, -600.0f}, 5.0);
    //----------------------------------------------------------------------------------

    // Scenery
    std::vector<SceneryController> sceneries;
    sceneries.reserve(6);

    Texture2D tree = LoadTexture("assets/textures/scenery/tree.png");
    Texture2D palm_tree = LoadTexture("assets/textures/scenery/palm_tree.png");

    //Right road scenery
    sceneries.emplace_back(tree, Vector2{road.GetPosition().x + road.GetWidth() - 45.0f, -50.0f}, 5.0);
    sceneries.emplace_back(palm_tree, Vector2{road.GetPosition().x + road.GetWidth() - 45.0f, -350.0f}, 5.0);
    //Left road scenery
    sceneries.emplace_back(palm_tree, Vector2{road.GetPosition().x - 15, -200.0f}, 5.0);
    sceneries.emplace_back(tree, Vector2{road.GetPosition().x - 15, -500.0f}, 5.0);


    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        car_controller.Update(road.GetPosition(), road.GetWidth());
        for (auto& npc_car : npc_cars)
        {
            npc_car.Update(road.GetPosition(), road.GetWidth());
        }

        for (auto& scenery : sceneries)
        {
            scenery.Update();
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        road.Draw();

        car_controller.Draw();

        for (const auto& npc_car : npc_cars)
        {
            npc_car.Draw();
        }

        for (const auto& scenery : sceneries)
        {
            scenery.Draw();
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
