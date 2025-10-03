#include "../include/Game.h"
#include "raylib.h"

Game::Game()
{
    Init();
}

Game::~Game()
{
    Cleanup();
}

void Game::Init()
{
    InitWindow(1280, 720, "Pixel Highway Minimal Test");
    SetTargetFPS(60);

    m_car_controller.InitResources();

    m_road.InitResources();

    CreateNpcCars();

    CreateScenery();
}

void Game::Cleanup()
{
    m_car_controller.UnloadResources();
    m_road.UnloadResources();

    UnloadTexture(m_blue_car);
    UnloadTexture(m_green_car);
    UnloadTexture(m_grey_car);
    UnloadTexture(m_pink_car);
    UnloadTexture(m_wreck_car);
    UnloadTexture(m_yellow_car);

    UnloadTexture(m_tree);
    UnloadTexture(m_tree);

    CloseWindow();
}

void Game::Run()
{
    while (!WindowShouldClose())
    {
        // update
        m_car_controller.Update(m_road.GetPosition(),m_road.GetWidth());

        for (auto& npc_car : m_npc_cars)
        {
            npc_car.Update(m_road.GetPosition(), m_road.GetWidth());

            if (CheckCollisionRecs(m_car_controller.getRect(), npc_car.getRect()))
            {
              //  std::cout<<"Game Over"<<std::endl;
            }
        }

        for (auto& scenery : m_sceneries)
        {
            scenery.Update();
        }

        // draw
        BeginDrawing();
        ClearBackground(BLACK);

        m_road.Draw();


        for (const auto& npc_car : m_npc_cars)
        {
            npc_car.Draw();
        }

        m_car_controller.Draw();

        for (const auto& scenery : m_sceneries)
        {
            scenery.Draw();
        }

        EndDrawing();
    }
}

void Game::CreateNpcCars()
{
    m_npc_cars.reserve(6);

     m_blue_car = LoadTexture("assets/textures/car/blue_car.png");
     m_green_car = LoadTexture("assets/textures/car/green_car.png");
     m_yellow_car = LoadTexture("assets/textures/car/yellow_car.png");
     m_pink_car = LoadTexture("assets/textures/car/pink_car.png");

     m_grey_car = LoadTexture("assets/textures/car/grey_car.png");
     m_wreck_car = LoadTexture("assets/textures/car/wreck_car.png");

    m_npc_cars.emplace_back(m_blue_car, Vector2{GetScreenWidth() / 1.85f, -50.0f}, 5.0);
    m_npc_cars.emplace_back(m_green_car, Vector2{GetScreenWidth() / 2.0f, -130.0f}, 5.0);
    m_npc_cars.emplace_back(m_yellow_car, Vector2{GetScreenWidth() / 2.4f, -250.0f}, 5.0);
    m_npc_cars.emplace_back(m_pink_car, Vector2{GetScreenWidth() / 2.25f, -400.0f}, 5.0);
    m_npc_cars.emplace_back(m_grey_car, Vector2{GetScreenWidth() / 2.25f, -500.0f}, 5.0);
    m_npc_cars.emplace_back(m_wreck_car, Vector2{GetScreenWidth() / 2.25f, -600.0f}, 5.0);
}

void Game::CreateScenery()
{
    m_sceneries.reserve(4);

    m_tree = LoadTexture("assets/textures/scenery/tree.png");
    m_palm_tree = LoadTexture("assets/textures/scenery/palm_tree.png");

    //Right road scenery
    m_sceneries.emplace_back(m_tree, Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -50.0f}, 5.0);
    m_sceneries.emplace_back(m_palm_tree, Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -350.0f}, 5.0);
    //Left road scenery
    m_sceneries.emplace_back(m_palm_tree, Vector2{m_road.GetPosition().x - 15, -200.0f}, 5.0);
    m_sceneries.emplace_back(m_tree, Vector2{m_road.GetPosition().x - 15, -500.0f}, 5.0);
}

