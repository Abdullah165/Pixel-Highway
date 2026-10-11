#ifndef PIXEL_HIGHWAY_SCOREMANAGER_H
#define PIXEL_HIGHWAY_SCOREMANAGER_H

#include "raylib.h"

class ScoreManager
{
public:
    ScoreManager();

    void Update();
    void Draw(const Font& font) const;
    void ResetCurrentScore();

    // Near-miss combo
    int RegisterNearMiss(); // adds the bonus and returns how many points it gave
    void ResetCombo();
    int GetMultiplier() const;

    // Saves the best score, but only if it changed. Call it when a run ends.
    void SaveBestScore();

private:
    int LoadBestScore() const;
    void WriteBestScore(int bestScore) const;
    void UpdateBest();

    static constexpr int NEAR_MISS_BONUS = 50;   // points at x1
    static constexpr int MAX_MULTIPLIER = 5;
    static constexpr float COMBO_TIME = 5.0f;    // seconds to get the next close call

    float m_currentScore = 0.0f;
    int m_bestScore = 0;
    bool m_bestScoreChanged = false;

    int m_combo = 0;           // close calls in a row
    float m_comboTimer = 0.0f; // seconds left before the combo drops
};


#endif //PIXEL_HIGHWAY_SCOREMANAGER_H