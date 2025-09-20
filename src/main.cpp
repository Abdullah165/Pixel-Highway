#include "raylib.h"
#include "../include/CarController.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Pixel Highway Example");

    SetTargetFPS(60);

    CarController car_controller;


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
