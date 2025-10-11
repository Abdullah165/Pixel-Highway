
#include "../include/ScoreManager.h"

#include <fstream>
#include <iostream>
#include "raylib.h"


ScoreManager::ScoreManager()
{
    m_bestScore = LoadBestScore("best_score.txt");
}

void ScoreManager::Draw() const
{
    DrawText(TextFormat("Score: %i", static_cast<int>(m_currentScore)),300, 20, 30, WHITE);
    DrawText(TextFormat("Best Score: %i", static_cast<int>(m_bestScore)),GetScreenWidth() / 1.6f, 20, 30, WHITE);
}

void ScoreManager::Update()
{
    m_currentScore += GetFrameTime() * 10;

    if (m_currentScore > m_bestScore)
    {
        m_bestScore = m_currentScore;
        SaveBestScore("best_score.txt", m_bestScore);
    }
}

int ScoreManager::LoadBestScore(const std::string& filename)
{
    std::ifstream file(filename);
    int bestScore = 0;

    if (file.is_open())
    {
        file >> bestScore;
        file.close();
    }

    return bestScore;
}

void ScoreManager::SaveBestScore(const std::string& filename, int bestScore)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << bestScore;
        file.close();
    }
}