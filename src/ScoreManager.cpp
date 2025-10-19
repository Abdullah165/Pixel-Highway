#include "../include/ScoreManager.h"

#include <fstream>
#include <iostream>
#include "raylib.h"


ScoreManager::ScoreManager()
{
    m_bestScore = LoadBestScore("best_score.txt");
}

void ScoreManager::ResetCurrentScore()
{
    m_currentScore = 0;
}


void ScoreManager::Draw(const Font& font) const
{
    DrawTextEx(font, TextFormat("Score: %i", static_cast<int>(m_currentScore)), (Vector2){300, 20.0f}, 30, 5,WHITE);

    DrawTextEx(font, TextFormat("Best Score: %i", static_cast<int>(m_bestScore)),
               (Vector2){GetScreenWidth() / 1.6f, 20.0f}, 30, 5,WHITE);
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
