#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>

#include "CarController.h"
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

private:
    void Init();
    void Cleanup();

    void CreateNpcCars();
    void CreateScenery();

private:
    CarController m_car_controller;
    Road m_road;

    std::vector<NpcCarController> m_npc_cars;

    std::vector<SceneryController> m_sceneries;

    // Npc cars and sceneries textures.
    std::map<std::string,Texture2D> m_textures;

    SoundManager m_sound;

    ScoreManager m_score;

    float m_worldSpeed = 3.0f;
    float m_worldSpeedTimer = 0.0f;
    const float WORLD_SPEED_INTERVAL = 15.0f;
    const float MAX_WORLD_SPEED = 10.0f;
};

#endif // GAME_H
