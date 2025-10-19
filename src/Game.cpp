#include "../include/Game.h"

#include <iostream>

#include "raylib.h"

Game::Game() : m_isGameOver(false)
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

    m_font = LoadFont("assets/fonts/JungleAdventurer.otf");

    m_car_controller.InitResources();

    m_road.InitResources();

    CreateNpcCars();

    CreateScenery();

    m_sound.InitResources();

    m_gameOver.Init(m_road.GetPosition().x);
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

    UnloadFont(m_font);

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

        if (m_isGameOver == false)
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
                // std::cout<<"Game Over"<<std::endl;
                m_isGameOver = true;
                m_sound.StartPlayingSound("CarCrash");
                m_worldSpeed = 0.0f;

                break;
            }
        }

        for (auto& scenery : m_sceneries)
        {
            scenery.Update(m_worldSpeed);
        }

        if (!m_isGameOver)
            m_score.Update();

        //Play again button
        if (CheckCollisionPointRec(GetMousePosition(), m_gameOver.GetPlayAgainButtonRec()))
        {
            m_gameOver.ChangePlayAgainTextColor(WHITE);

            if (IsMouseButtonPressed(0))
            {
                ResetGame();
            }
        }
        else
        {
            m_gameOver.ChangePlayAgainTextColor(RED);
        }

        // draw
        BeginDrawing();
        ClearBackground(DARKGRAY);

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

        m_score.Draw(m_font);

        if (m_isGameOver)
            m_gameOver.Draw(m_road.GetPosition().x, m_road.GetWidth(), m_font);

        EndDrawing();
    }
}

void Game::ResetGame()
{
    m_isGameOver = false;

    m_worldSpeed = INT_WORLD_SPEED;

    m_score.ResetCurrentScore();

    //Reset NPCs cars and Sceneries
    m_npc_cars[0].SetPosition(Vector2{GetScreenWidth() / 1.85f, -50.0f});
    m_npc_cars[1].SetPosition(Vector2{GetScreenWidth() / 2.0f, -150.0f});
    m_npc_cars[2].SetPosition(Vector2{GetScreenWidth() / 2.4f, -250.0f});
    m_npc_cars[3].SetPosition(Vector2{GetScreenWidth() / 2.25f, -400.0f});
    m_npc_cars[4].SetPosition(Vector2{GetScreenWidth() / 2.26f, -500.0f});
    m_npc_cars[5].SetPosition(Vector2{GetScreenWidth() / 2.25f, -600.0f});

    m_sceneries[0].SetPosition(Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -50.0f});
    m_sceneries[1].SetPosition(Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -350.0f});
    m_sceneries[2].SetPosition(Vector2{m_road.GetPosition().x - 15, -200.0f});
    m_sceneries[3].SetPosition(Vector2{m_road.GetPosition().x - 15, -500.0f});
}

void Game::CreateNpcCars()
{
    auto& blue_car = m_textures.emplace(BlueCar, LoadTexture("assets/textures/car/blue_car.png")).first->second;
    auto& green_car = m_textures.emplace(GreenCar, LoadTexture("assets/textures/car/green_car.png")).first->second;
    auto& yellow_car = m_textures.emplace(YellowCar, LoadTexture("assets/textures/car/yellow_car.png")).first->
                                  second;
    auto& pink_car = m_textures.emplace(PinkCar, LoadTexture("assets/textures/car/pink_car.png")).first->second;
    auto& grey_car = m_textures.emplace(GreyCar, LoadTexture("assets/textures/car/grey_car.png")).first->second;
    auto& wreck_car = m_textures.emplace(WreckCar, LoadTexture("assets/textures/car/wreck_car.png")).first->second;


    m_npc_cars[0] = NpcCarController(&blue_car, Vector2{GetScreenWidth() / 1.85f, -50.0f});
    m_npc_cars[1] = NpcCarController(&green_car, Vector2{GetScreenWidth() / 2.0f, -150.0f});
    m_npc_cars[2] = NpcCarController(&yellow_car, Vector2{GetScreenWidth() / 2.4f, -250.0f});
    m_npc_cars[3] = NpcCarController(&pink_car, Vector2{GetScreenWidth() / 2.25f, -400.0f});
    m_npc_cars[4] = NpcCarController(&grey_car, Vector2{GetScreenWidth() / 2.26f, -500.0f});
    m_npc_cars[5] = NpcCarController(&wreck_car, Vector2{GetScreenWidth() / 2.25f, -600.0f});
}

void Game::CreateScenery()
{
    auto& tree = m_textures.emplace(Tree, LoadTexture("assets/textures/scenery/tree.png")).first->second;
    auto& palm_tree = m_textures.emplace(PalmTree, LoadTexture("assets/textures/scenery/palm_tree.png")).first->
                                 second;

    //Right road scenery
    m_sceneries[0] = SceneryController(&tree, Vector2{m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -50.0f});
    m_sceneries[1] = SceneryController(&palm_tree, Vector2{
                                           m_road.GetPosition().x + m_road.GetWidth() - 45.0f, -350.0f
                                       });
    //Left road scenery
    m_sceneries[2] = SceneryController(&palm_tree, Vector2{m_road.GetPosition().x - 15, -200.0f});
    m_sceneries[3] = SceneryController(&tree, Vector2{m_road.GetPosition().x - 15, -500.0f});
}
