#include "raylib.h"
#include "../include/CarController.h"
#include "../include/Road.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Pixel Highway Example");

    SetTargetFPS(60);

    CarController car_controller;

    Road road;

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        car_controller.Update();
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        road.Draw();

        car_controller.Draw();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------
    return 0;
}
