#include "../include/Game.h"

#include <iostream>

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

    m_sound.InitResources();
}

void Game::Cleanup()
{
    m_car_controller.UnloadResources();
    m_road.UnloadResources();

    for (auto& [key,texture] : m_textures)
    {
        UnloadTexture(texture);
    }
    m_textures.clear();

    m_sound.ShutdownAllSounds();

    CloseWindow();
}

void Game::Run()
{
    while (!WindowShouldClose())
    {
        //Sounds
        if (m_sound.IsSoundStillPlaying("Racing") == false)
            m_sound.StartPlayingSound("Racing");

        if (m_sound.IsSoundStillPlaying("Traffic") == false)
            m_sound.StartPlayingSound("Traffic");

        // update
        m_car_controller.Update(m_road.GetPosition(), m_road.GetWidth());

        // Gradually increase the world speed every WORLD_SPEED_INTERVAL seconds,
        // until reaching MAX_WORLD_SPEED.
        m_worldSpeedTimer += GetFrameTime();
        if (m_worldSpeedTimer > WORLD_SPEED_INTERVAL)
        {
            m_worldSpeed = std::min(m_worldSpeed + 1, MAX_WORLD_SPEED);

            m_worldSpeedTimer = 0.0f;
            // std::cout << "World Speed = " << m_worldSpeed << std::endl;
        }

        for (auto& npc_car : m_npc_cars)
        {
            npc_car.Update(m_road.GetPosition(), m_road.GetWidth(), m_worldSpeed);

            if (CheckCollisionRecs(m_car_controller.getRect(), npc_car.getRect()))
            {
                //  std::cout<<"Game Over"<<std::endl;
                m_sound.StartPlayingSound("CarCrash");
                break;
            }
        }

        for (auto& scenery : m_sceneries)
        {
            scenery.Update(m_worldSpeed);
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

    auto& blue_car = m_textures.emplace("blue car", LoadTexture("assets/textures/car/blue_car.png")).first->second;
    auto& green_car = m_textures.emplace("green car", LoadTexture("assets/textures/car/green_car.png")).first->second;
    auto& yellow_car = m_textures.emplace("yellow car", LoadTexture("assets/textures/car/yellow_car.png")).first->
                                  second;
    auto& pink_car = m_textures.emplace("pink car", LoadTexture("assets/textures/car/pink_car.png")).first->second;
    auto& grey_car = m_textures.emplace("grey car", LoadTexture("assets/textures/car/grey_car.png")).first->second;
    auto& wreck_car = m_textures.emplace("wreck car", LoadTexture("assets/textures/car/wreck_car.png")).first->second;


    m_npc_cars.emplace_back(blue_car, Vector2{GetScreenWidth() / 1.85f, -50.0f}, m_worldSpeed);
    m_npc_cars.emplace_back(green_car, Vector2{GetScreenWidth() / 2.0f, -130.0f}, m_worldSpeed);
    m_npc_cars.emplace_back(yellow_car, Vector2{GetScreenWidth() / 2.4f, -250.0f}, m_worldSpeed);
    m_npc_cars.emplace_back(pink_car, Vector2{GetScreenWidth() / 2.25f, -400.0f}, m_worldSpeed);
    m_npc_cars.emplace_back(grey_car, Vector2{GetScreenWidth() / 2.25f, -500.0f}, m_worldSpeed);
    m_npc_cars.emplace_back(wreck_car, Vector2{GetScreenWidth() / 2.25f, -600.0f}, m_worldSpeed);
}

void Game::CreateScenery()
{
    m_sceneries.reserve(4);

    auto& tree = m_textures.emplace("tree", LoadTexture("assets/textures/scenery/tree.png")).first->second;
    auto& palm_tree = m_textures.emplace("palm tree", LoadTexture("assets/textures/scenery/palm_tree.png")).first->
                                 second;

    //Right road scenery
    m_sceneries.emplace_back(tree, Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -50.0f}, m_worldSpeed);
    m_sceneries.emplace_back(palm_tree, Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -350.0f},
                             m_worldSpeed);
    //Left road scenery
    m_sceneries.emplace_back(palm_tree, Vector2{m_road.GetPosition().x - 15, -200.0f}, m_worldSpeed);
    m_sceneries.emplace_back(tree, Vector2{m_road.GetPosition().x - 15, -500.0f}, m_worldSpeed);
}
