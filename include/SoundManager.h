#ifndef PIXEL_HIGHWAY_SOUNDMANAGER_H
#define PIXEL_HIGHWAY_SOUNDMANAGER_H
#include <map>
#include <string>

#include "raylib.h"


class SoundManager
{
public:
    void InitResources();
    void StartPlayingSound(const std::string& key);
    void ShutdownAllSounds();

    bool IsSoundStillPlaying(const std::string& key);
private:
    std::map<std::string, Sound> m_sounds;
};


#endif //PIXEL_HIGHWAY_SOUNDMANAGER_H
