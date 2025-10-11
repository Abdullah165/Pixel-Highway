#ifndef PIXEL_HIGHWAY_SCOREMANAGER_H
#define PIXEL_HIGHWAY_SCOREMANAGER_H
#include <string>

class ScoreManager
{
public:
    ScoreManager();
    void Draw() const;
    void Update();
    int LoadBestScore(const std::string& filename);
    void SaveBestScore(const std::string& filename, int bestScore);
private:
    float m_currentScore = 0.0f;
    int m_bestScore = 0;
};


#endif //PIXEL_HIGHWAY_SCOREMANAGER_H
