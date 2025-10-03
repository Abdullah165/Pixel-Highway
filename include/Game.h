#ifndef GAME_H
#define GAME_H

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
    Texture2D m_blue_car, m_green_car, m_yellow_car, m_pink_car, m_grey_car, m_wreck_car;

    std::vector<SceneryController> m_sceneries;
    Texture2D m_tree;
    Texture2D m_palm_tree;
};

#endif // GAME_H
