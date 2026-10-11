#include "../include/Game.h"

#include <ctime>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

namespace
{
    // Must stay in the same order as Game::TextureID.
    constexpr std::array<const char*, Game::TextureCount> TEXTURE_FILES = {
        "assets/textures/car/blue_car.png",
        "assets/textures/car/green_car.png",
        "assets/textures/car/yellow_car.png",
        "assets/textures/car/pink_car.png",
        "assets/textures/car/grey_car.png",
        "assets/textures/car/wreck_car.png",
        "assets/textures/scenery/tree.png",
        "assets/textures/scenery/palm_tree.png"
    };
}

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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Highway");
    SetTargetFPS(60);
    SetRandomSeed(static_cast<unsigned int>(std::time(nullptr)));

    m_font = LoadFont("assets/fonts/JungleAdventurer.otf");

    for (int i = 0; i < TextureCount; ++i)
    {
        m_textures[i] = LoadTexture(TEXTURE_FILES[i]);
    }

    m_car_controller.InitResources();
    m_road.InitResources();

    CreateNpcCars();
    CreateScenery();

    m_gameOver.Init(m_road.GetPosition().x, static_cast<float>(m_road.GetWidth()));
}

void Game::Cleanup()
{
    m_car_controller.UnloadResources();
    m_road.UnloadResources();

    for (auto& texture : m_textures)
    {
        UnloadTexture(texture);
    }

    m_sound.ShutdownAllSounds();

    UnloadFont(m_font);

    CloseWindow();
}

void Game::Run()
{
    // Everything is loaded: tell CrazyGames the loading screen can go away.
    CrazySDK::LoadingFinished();

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(
        [](void* game) { static_cast<Game*>(game)->UpdateDrawFrame(); }, this, 0, 1);
#else
    while (!WindowShouldClose())
        UpdateDrawFrame();
#endif
}

void Game::UpdateDrawFrame()
{
    // The browser blocks audio until the player interacts with the page,
    // so we wait for the first click / key press before starting.
    if (!m_started)
    {
        DrawStartScreen();
        return;
    }

    // While an ad is playing the game is frozen (the ad is shown on top of the page).
    if (CrazySDK::IsAdPlaying())
    {
        DrawGame();
        return;
    }

    // dt = 1.0 means "one frame at 60 FPS". This keeps the game speed the same
    // on 60 Hz, 144 Hz and slower computers.
    const float dt = std::min(GetFrameTime() * 60.0f, MAX_FRAME_SCALE);

    UpdateGame(dt);
    DrawGame();
}

void Game::DrawStartScreen()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || GetKeyPressed() != 0)
    {
        m_sound.InitResources(); // audio starts AFTER the user's click
        m_started = true;
        CrazySDK::GameplayStart();
    }

    BeginDrawing();
    ClearBackground(BLACK);

    const char* text = "Click to Start";
    const Vector2 size = MeasureTextEx(m_font, text, 40, 4);
    DrawTextEx(m_font, text,
        Vector2{ (SCREEN_WIDTH - size.x) / 2.0f, (SCREEN_HEIGHT - size.y) / 2.0f },
        40, 4, WHITE);

    EndDrawing();
}

void Game::UpdateGame(float dt)
{
    const float frameTime = GetFrameTime();

    if (m_noAdMessageTimer > 0.0f)
        m_noAdMessageTimer -= frameTime;

    m_popups.Update(frameTime);

    if (!m_isGameOver)
    {
        // Sounds
        if (!m_sound.IsSoundStillPlaying("Racing"))
            m_sound.StartPlayingSound("Racing");

        if (!m_sound.IsSoundStillPlaying("Traffic"))
            m_sound.StartPlayingSound("Traffic");

        m_car_controller.Update(m_road.GetPosition(), m_road.GetWidth(), dt);

        if (m_invincibleTimer > 0.0f)
            m_invincibleTimer -= frameTime;

        // Gradually increase the world speed every WORLD_SPEED_INTERVAL seconds,
        // until reaching MAX_WORLD_SPEED.
        m_worldSpeedTimer += frameTime;
        if (m_worldSpeedTimer > WORLD_SPEED_INTERVAL)
        {
            m_worldSpeed = std::min(m_worldSpeed + 1.0f, MAX_WORLD_SPEED);
            m_worldSpeedTimer = 0.0f;
        }

        for (auto& npc_car : m_npc_cars)
        {
            npc_car.Update(m_road.GetPosition(), m_road.GetWidth(), m_worldSpeed * dt);

            if (m_invincibleTimer <= 0.0f &&
                CheckCollisionRecs(m_car_controller.getRect(), npc_car.getRect()))
            {
                m_car_controller.PlayCarExplosionVFX();
                m_sound.StartPlayingSound("CarCrash");
                m_isGameOver = true;
                m_speedBeforeCrash = m_worldSpeed; // remembered for "Continue"
                m_worldSpeed = 0.0f;
                m_score.ResetCombo(); // a crash breaks the combo
                CrazySDK::GameplayStop();
                break;
            }
        }

        // Close calls are only counted while the player is still alive.
        if (!m_isGameOver)
            CheckNearMisses();

        for (auto& scenery : m_sceneries)
        {
            scenery.Update(m_worldSpeed * dt);
        }

        m_score.Update();

        if (m_isGameOver)
            m_score.SaveBestScore(); // save once, when the run ends
    }
    else
    {
        // Game Over buttons (only active on the Game Over screen)
        const Vector2 mouse = GetMousePosition();

        const bool hoverPlayAgain = CheckCollisionPointRec(mouse, m_gameOver.GetPlayAgainButtonRec());
        const bool hoverContinue = !m_continueUsed &&
            CheckCollisionPointRec(mouse, m_gameOver.GetContinueButtonRec());

        m_gameOver.ChangePlayAgainTextColor(hoverPlayAgain ? WHITE : RED);
        m_gameOver.ChangeContinueTextColor(hoverContinue ? WHITE : RED);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (hoverContinue)
                OnContinuePressed();
            else if (hoverPlayAgain)
                OnPlayAgainPressed();
        }
    }
}


void Game::OnContinuePressed()
{
    m_sound.StartPlayingSound("ButtonClick");

    // Rewarded ad: the player chose to watch it, the reward is another chance.
    CrazySDK::RequestAd(CrazySDK::AdType::Rewarded, [this](bool completed)
    {
        if (completed)
            ContinueRun();
        else
            m_noAdMessageTimer = 3.0f; // ad failed / not available
    });
}

void Game::OnPlayAgainPressed()
{
    m_sound.StartPlayingSound("ButtonClick");

    // The player already watched a rewarded ad in this run: no second ad right away.
    if (m_continueUsed)
    {
        ResetGame();
        return;
    }

    // Midgame ad at the natural break. The new run starts when the ad ends,
    // even if no ad was available.
    CrazySDK::RequestAd(CrazySDK::AdType::Midgame, [this](bool)
    {
        ResetGame();
    });
}

void Game::ContinueRun()
{
    m_continueUsed = true;
    m_isGameOver = false;
    m_worldSpeed = m_speedBeforeCrash;
    m_invincibleTimer = CONTINUE_PROTECTION_TIME;

    m_car_controller.StopExplosionVFX();
    PlaceNpcCars(); // move the traffic above the screen so the road ahead is clear

    CrazySDK::GameplayStart();
}

void Game::CheckNearMisses()
{
    const Rectangle carRect = m_car_controller.getRect();

    // The "danger zone": the car's hitbox plus a margin on every side.
    const Rectangle dangerZone = Rectangle{
        carRect.x - NEAR_MISS_MARGIN_X,
        carRect.y - NEAR_MISS_MARGIN_Y,
        carRect.width + NEAR_MISS_MARGIN_X * 2.0f,
        carRect.height + NEAR_MISS_MARGIN_Y * 2.0f
    };
    const float carBottom = carRect.y + carRect.height;

    for (auto& npc_car : m_npc_cars)
    {
        if (npc_car.IsNearMissHandled())
            continue;

        const Rectangle npcRect = npc_car.getRect();

        // Touching the car (possible during the Continue protection) is never a close call.
        if (CheckCollisionRecs(carRect, npcRect))
        {
            npc_car.MarkNearMissHandled();
            continue;
        }

        // Did this car come close to the player?
        if (CheckCollisionRecs(dangerZone, npcRect))
            npc_car.SetWasNear(true);

        // The car was close and has now passed the player: reward it.
        if (npc_car.WasNear() && npcRect.y > carBottom)
        {
            const int bonus = m_score.RegisterNearMiss();

            m_popups.Add(TextFormat("CLOSE CALL +%i", bonus),
                Vector2{ carRect.x + carRect.width / 2.0f, carRect.y - 20.0f },
                YELLOW);

            npc_car.MarkNearMissHandled();
        }
    }
}

void Game::DrawGame()
{
    BeginDrawing();
    ClearBackground(m_selectedBackgroundColor);

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

    m_popups.Draw(m_font);
    m_score.Draw(m_font);

    if (m_isGameOver)
    {
        m_gameOver.Draw(m_font, !m_continueUsed);

        if (m_noAdMessageTimer > 0.0f)
        {
            const char* message = "No ad available";
            const Vector2 size = MeasureTextEx(m_font, message, 22, 3);
            const float roadX = m_road.GetPosition().x;
            DrawTextEx(m_font, message,
                Vector2{ roadX + (m_road.GetWidth() - size.x) / 2.0f, 470.0f }, 22, 3, WHITE);
        }
    }

    EndDrawing();
}

void Game::ResetGame()
{
    m_isGameOver = false;
    m_worldSpeed = START_WORLD_SPEED;
    m_worldSpeedTimer = 0.0f;

    m_continueUsed = false;
    m_invincibleTimer = 0.0f;
    m_noAdMessageTimer = 0.0f;

    m_car_controller.StopExplosionVFX();
    m_score.ResetCurrentScore(); // also resets the combo
    m_popups.Clear();
    m_selectedBackgroundColor = GetRandomBackgroundColor();

    PlaceNpcCars();
    PlaceSceneries();

    CrazySDK::GameplayStart();
}

Color Game::GetRandomBackgroundColor() const
{
    return m_backgroundColors[GetRandomValue(0, static_cast<int>(m_backgroundColors.size()) - 1)];
}

void Game::CreateNpcCars()
{
    // BlueCar..WreckCar are the first 6 textures.
    for (size_t i = 0; i < m_npc_cars.size(); ++i)
    {
        m_npc_cars[i] = NpcCarController(&m_textures[BlueCar + i], Vector2{ 0.0f, 0.0f });
    }

    PlaceNpcCars();
}

void Game::CreateScenery()
{
    m_sceneries[0] = SceneryController(&m_textures[Tree], Vector2{ 0.0f, 0.0f });     // right side
    m_sceneries[1] = SceneryController(&m_textures[PalmTree], Vector2{ 0.0f, 0.0f }); // right side
    m_sceneries[2] = SceneryController(&m_textures[PalmTree], Vector2{ 0.0f, 0.0f }); // left side
    m_sceneries[3] = SceneryController(&m_textures[Tree], Vector2{ 0.0f, 0.0f });     // left side

    PlaceSceneries();
}

void Game::PlaceNpcCars()
{
    // x = screen width / divisor, y = start height above the screen
    static constexpr float X_DIVISOR[6] = { 1.85f, 2.0f, 2.4f, 2.25f, 2.26f, 2.25f };
    static constexpr float START_Y[6] = { -50.0f, -150.0f, -250.0f, -350.0f, -450.0f, -550.0f };

    for (size_t i = 0; i < m_npc_cars.size(); ++i)
    {
        m_npc_cars[i].SetPosition(Vector2{ SCREEN_WIDTH / X_DIVISOR[i], START_Y[i] });
    }
}

void Game::PlaceSceneries()
{
    const float roadX = m_road.GetPosition().x;
    const float roadWidth = static_cast<float>(m_road.GetWidth());

    // Right side of the road
    m_sceneries[0].SetPosition(Vector2{ roadX + roadWidth - 45.0f, -50.0f });
    m_sceneries[1].SetPosition(Vector2{ roadX + roadWidth - 45.0f, -350.0f });

    // Left side of the road
    m_sceneries[2].SetPosition(Vector2{ roadX - 15.0f, -200.0f });
    m_sceneries[3].SetPosition(Vector2{ roadX - 15.0f, -500.0f });
}