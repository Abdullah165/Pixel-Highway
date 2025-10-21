#ifndef GAME_H
#define GAME_H

#include <map>

#include "CarController.h"
#include "GameOver.h"
#include "NpcCarController.h"
#include "Road.h"
#include "SceneryController.h"
#include "ScoreManager.h"
#include "SoundManager.h"

class Game
{
public:
    Game();
    ~Game();

    void Run();

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
        PalmTree
    };
private:
    void Init();
    void Cleanup();

    void CreateNpcCars();
    void CreateScenery();
    void ResetGame();
    Color GetRandomBackgroundColor() const;

private:
    CarController m_car_controller;
    Road m_road;

    std::array<NpcCarController, 6> m_npc_cars;

    std::array<SceneryController, 4> m_sceneries;

    // Npc cars and sceneries textures.
    std::map<TextureID, Texture2D> m_textures;

    SoundManager m_sound;

    ScoreManager m_score;

    GameOver m_gameOver;

    const float INT_WORLD_SPEED = 3.0f;
    float m_worldSpeed = 3.0f;
    float m_worldSpeedTimer = 0.0f;
    const float WORLD_SPEED_INTERVAL = 15.0f;
    const float MAX_WORLD_SPEED = 10.0f;

    Font m_font;

    std::array<Color,4> m_randomBackGroundColor;
    Color m_selectedBackgroundColor;

    bool m_isGameOver;
};

#endif // GAME_H
