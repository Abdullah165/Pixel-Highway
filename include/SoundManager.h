#ifndef PIXEL_HIGHWAY_SOUNDMANAGER_H
#define PIXEL_HIGHWAY_SOUNDMANAGER_H
#include <string>
#include <unordered_map>

#include "raylib.h"


class SoundManager
{
public:
    void InitResources();
    void StartPlayingSound(const std::string& key);
    bool IsSoundStillPlaying(const std::string& key) const;
    void ShutdownAllSounds();

private:
    std::unordered_map<std::string, Sound> m_sounds;
};


#endif //PIXEL_HIGHWAY_SOUNDMANAGER_H