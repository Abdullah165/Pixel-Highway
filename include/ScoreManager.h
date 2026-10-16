#ifndef PIXEL_HIGHWAY_SCOREMANAGER_H
#define PIXEL_HIGHWAY_SCOREMANAGER_H

#include <array>

#include "Difficulty.h"
#include "raylib.h"

class ScoreManager
{
public:
    ScoreManager();

    // Switches to the best score of that difficulty. Call it before each run starts.
    void SetDifficulty(Difficulty difficulty);

    void Update();
    void Draw(const Font& font) const;
    void ResetCurrentScore();

    // Near-miss combo
    int RegisterNearMiss(); // adds the bonus and returns how many points it gave
    void ResetCombo();
    int GetMultiplier() const;

    // Saves the best score of the current difficulty, but only if it changed.
    void SaveBestScore();
    // Best score of any difficulty (used by the start screen)
    int GetBestScore(Difficulty difficulty) const;

private:
    static int ToIndex(Difficulty difficulty);
    static const char* ToName(Difficulty difficulty);

    int LoadBestScore(int index) const;
    void WriteBestScore(int index, int bestScore) const;
    void UpdateBest();

    static constexpr int NEAR_MISS_BONUS = 50;   // points at x1
    static constexpr int MAX_MULTIPLIER = 5;
    static constexpr float COMBO_TIME = 5.0f;    // seconds to get the next close call

    float m_currentScore = 0.0f;

    Difficulty m_difficulty = Difficulty::Medium;
    std::array<int, 3> m_bestScores{}; // Easy, Medium, Hard
    bool m_bestScoreChanged = false;

    int m_combo = 0;           // close calls in a row
    float m_comboTimer = 0.0f; // seconds left before the combo drops
};


#endif //PIXEL_HIGHWAY_SCOREMANAGER_H