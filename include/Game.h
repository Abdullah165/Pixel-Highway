#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <array>

#include "CarController.h"
#include "CrazySDK.h"
#include "GameOver.h"
#include "NpcCarController.h"
#include "Road.h"
#include "SceneryController.h"
#include "ScoreManager.h"
#include "SoundManager.h"
#include "PopupManager.h"
#include "ScreenShake.h"
#include "Difficulty.h"
#include "CoinManager.h"

class Game
{
public:
    Game();
    ~Game();

    void Run();
    void UpdateDrawFrame(); // One frame. The main loop calls this again and again.

    // The order must match TEXTURE_FILES in Game.cpp.
    enum TextureID
    {
        // NPC cars
        BlueCar,
        GreenCar,
        YellowCar,
        PinkCar,
        GreyCar,
        WreckCar,

        // Scenery
        Tree,
        PalmTree,

        TextureCount
    };

private:
    void Init();
    void Cleanup();

    void CreateNpcCars();
    void CreateScenery();
    void PlaceNpcCars();
    void PlaceSceneries();
    void ResetGame();
    void StartGame(Difficulty difficulty);
    void CheckNearMisses();
    void OnCrash();

    void UpdateGame(float dt);
    void DrawGame();
    void DrawStartScreen();

    // Game over buttons
    void OnContinuePressed(); // rewarded ad -> keep playing
    void OnPlayAgainPressed(); // midgame ad -> new run
    void OnMenuPressed();  // midgame ad -> back to the difficulty screen
    void ReturnToMenu();
    void ContinueRun();

    Color GetRandomBackgroundColor() const;

private:
    void DrawButton(const Font& font, const Rectangle& bounds, const char* text,
        Color fillColor, Color textColor) const;

    void OnDoubleCoinsPressed(); // rewarded ad -> bonus coins
    int GetDoubleCoinsOffer() const; // 0 = do not show the button

    static constexpr int SCREEN_WIDTH = 1280;
    static constexpr int SCREEN_HEIGHT = 720;

    static constexpr float START_WORLD_SPEED = 3.0f;
    static constexpr float MAX_WORLD_SPEED = 10.0f;
    static constexpr float WORLD_SPEED_INTERVAL = 15.0f;
    static constexpr float MAX_FRAME_SCALE = 3.0f; // protects against huge lag spikes
    static constexpr float CONTINUE_PROTECTION_TIME = 2.0f; // seconds without crashing after Continue
    // A "close call" is an NPC car touching this much extra space around your car's hitbox.
    static constexpr float NEAR_MISS_MARGIN_X = 18.0f;
    static constexpr float NEAR_MISS_MARGIN_Y = 10.0f;
    static constexpr float CRASH_SHAKE_DURATION = 0.6f;  // seconds of shaking
    static constexpr float CRASH_SHAKE_STRENGTH = 12.0f; // pixels at the start
    static constexpr float GAME_OVER_DELAY = 1.0f;       // seconds from crash to Game Over screen
    static constexpr int MIN_COINS_TO_OFFER_DOUBLE = 5; // smaller bonuses are not worth an ad

    CarController m_car_controller;
    Road m_road;

    std::array<NpcCarController, 6> m_npc_cars;
    std::array<SceneryController, 4> m_sceneries;

    // NPC cars and scenery textures (indexed by TextureID).
    std::array<Texture2D, TextureCount> m_textures{};

    SoundManager m_sound;
    ScoreManager m_score;
    GameOver m_gameOver;
    PopupManager m_popups;
    CoinManager m_coins;

    ScreenShake m_shake;
    Camera2D m_camera{ Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}, 0.0f, 1.0f };

    bool m_isCrashing = false; // crash sequence running (before the Game Over screen)
    bool m_watchedRewardedAd = false; // any rewarded ad watched in this run (Continue or Double Coins)
    float m_crashTimer = 0.0f;

    Font m_font{};

    std::array<Color, 4> m_backgroundColors{ DARKGRAY, BLACK, DARKBROWN, DARKGREEN };
    Color m_selectedBackgroundColor = DARKGRAY;

    float m_worldSpeed = START_WORLD_SPEED;
    float m_worldSpeedTimer = 0.0f;
    float m_speedBeforeCrash = START_WORLD_SPEED;

    bool m_isGameOver = false;
    bool m_started = false;

    bool m_continueUsed = false;     // one Continue per run
    float m_invincibleTimer = 0.0f;  // short protection after Continue
    float m_noAdMessageTimer = 0.0f; // shows "No ad available" for a moment

    // Difficulty selection
    Rectangle m_easyButtonBounds{};
    Rectangle m_mediumButtonBounds{};
    Rectangle m_hardButtonBounds{};
    Difficulty m_difficulty = Difficulty::Medium;
    float m_startSpeed = 5.0f; // world speed at the start of every run
};

#endif // GAME_H