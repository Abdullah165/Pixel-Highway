#include "../include/ScoreManager.h"

#include <algorithm>
#include <fstream>

#include "../include/TextUtils.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>

// Each difficulty has its own key: ph_best_easy, ph_best_medium, ph_best_hard.
// Easy also reads the old "ph_best" key once, so the old best score is not lost.
EM_JS(int, JS_LoadBest, (int index), {
    try {
        var names = ["easy", "medium", "hard"];
        var value = localStorage.getItem("ph_best_" + names[index]);
        if (value == = null && index == = 0) value = localStorage.getItem("ph_best");
        return parseInt(value || "0") || 0;
    }
 catch (e) { return 0; }
    });

EM_JS(void, JS_SaveBest, (int index, int v), {
    try {
        var names = ["easy", "medium", "hard"];
        localStorage.setItem("ph_best_" + names[index], v.toString());
    }
 catch (e) {}
    });
#else
namespace
{
    // Desktop only. Easy also reads the old file once, so the old best score is not lost.
    constexpr const char* BEST_SCORE_FILES[3] = {
        "best_score_easy.txt", "best_score_medium.txt", "best_score_hard.txt"
    };
    constexpr const char* OLD_BEST_SCORE_FILE = "best_score.txt";
}
#endif

ScoreManager::ScoreManager()
{
    for (int i = 0; i < static_cast<int>(m_bestScores.size()); ++i)
        m_bestScores[i] = LoadBestScore(i);
}

void ScoreManager::SetDifficulty(Difficulty difficulty)
{
    SaveBestScore(); // never lose a pending best score when switching
    m_difficulty = difficulty;
    m_bestScoreChanged = false;
}

void ScoreManager::ResetCurrentScore()
{
    m_currentScore = 0.0f;
    ResetCombo();
}

void ScoreManager::Update()
{
    const float frameTime = GetFrameTime();

    // The combo runs out if the player does not take another risk in time.
    if (m_combo > 0)
    {
        m_comboTimer -= frameTime;
        if (m_comboTimer <= 0.0f)
            ResetCombo();
    }

    m_currentScore += frameTime * 10.0f * GetMultiplier();

    UpdateBest();
}

int ScoreManager::RegisterNearMiss()
{
    const int bonus = NEAR_MISS_BONUS * GetMultiplier(); // paid at the multiplier you had

    m_currentScore += static_cast<float>(bonus);
    m_combo = std::min(m_combo + 1, MAX_MULTIPLIER - 1);
    m_comboTimer = COMBO_TIME;

    UpdateBest();
    return bonus;
}

void ScoreManager::ResetCombo()
{
    m_combo = 0;
    m_comboTimer = 0.0f;
}

int ScoreManager::GetMultiplier() const
{
    return std::min(1 + m_combo, MAX_MULTIPLIER);
}

void ScoreManager::Draw(const Font& font) const
{
    const int best = m_bestScores[ToIndex(m_difficulty)];
    const float bestX = GetScreenWidth() / 1.6f;

    DrawTextEx(font, TextFormat("Score: %i", static_cast<int>(m_currentScore)),
        Vector2{ 300.0f, 20.0f }, 30, 5, WHITE);

    DrawTextEx(font, TextFormat("Best Score: %i", best),
        Vector2{ bestX, 20.0f }, 30, 5, WHITE);

    // Which difficulty this best score belongs to
    DrawTextEx(font, ToName(m_difficulty), Vector2{ bestX, 58.0f }, 22, 3, LIGHTGRAY);

    // Combo text and a bar showing how much time is left
    if (m_combo > 0)
    {
        DrawTextOutlined(font, TextFormat("Combo x%i", GetMultiplier()),
            Vector2{ 300.0f, 60.0f }, 26, 4, ORANGE, BLACK);

        const int barWidth = 160;
        const float fraction = std::clamp(m_comboTimer / COMBO_TIME, 0.0f, 1.0f);
        DrawRectangle(300, 95, barWidth, 8, ColorAlpha(BLACK, 0.5f));
        DrawRectangle(300, 95, static_cast<int>(barWidth * fraction), 8, ORANGE);
    }
}

void ScoreManager::UpdateBest()
{
    int& best = m_bestScores[ToIndex(m_difficulty)];

    if (static_cast<int>(m_currentScore) > best)
    {
        best = static_cast<int>(m_currentScore);
        m_bestScoreChanged = true;
    }
}

void ScoreManager::SaveBestScore()
{
    if (!m_bestScoreChanged) return;

    const int index = ToIndex(m_difficulty);
    WriteBestScore(index, m_bestScores[index]);
    m_bestScoreChanged = false;
}

int ScoreManager::GetBestScore(Difficulty difficulty) const
{
    return m_bestScores[ToIndex(difficulty)];
}

int ScoreManager::ToIndex(Difficulty difficulty)
{
    switch (difficulty)
    {
    case Difficulty::Easy:   return 0;
    case Difficulty::Medium: return 1;
    case Difficulty::Hard:   return 2;
    }
    return 1;
}

const char* ScoreManager::ToName(Difficulty difficulty)
{
    switch (difficulty)
    {
    case Difficulty::Easy:   return "Easy";
    case Difficulty::Medium: return "Medium";
    case Difficulty::Hard:   return "Hard";
    }
    return "Medium";
}

int ScoreManager::LoadBestScore(int index) const
{
#if defined(__EMSCRIPTEN__)
    return JS_LoadBest(index);
#else
    std::ifstream file(BEST_SCORE_FILES[index]);
    if (!file.is_open() && index == 0)
        file.open(OLD_BEST_SCORE_FILE);

    int bestScore = 0;
    if (file.is_open())
        file >> bestScore;
    return bestScore;
#endif
}

void ScoreManager::WriteBestScore(int index, int bestScore) const
{
#if defined(__EMSCRIPTEN__)
    JS_SaveBest(index, bestScore);
#else
    std::ofstream file(BEST_SCORE_FILES[index]);
    if (file.is_open())
        file << bestScore;
#endif
}