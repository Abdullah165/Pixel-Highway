#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>

#include "CarController.h"
#include "NpcCarController.h"
#include "Road.h"
#include "SceneryController.h"

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
};

#endif // GAME_H
