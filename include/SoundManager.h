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
    void StopPlayingSound(const std::string& key);
    void PlaySoundWithPitch(const std::string& key, float pitch);
    bool IsSoundStillPlaying(const std::string& key) const;
    void ShutdownAllSounds();
    void SetPitch(const std::string& key, float pitch); // changes a sound's pitch, also while it plays
private:
    std::unordered_map<std::string, Sound> m_sounds;
};


#endif //PIXEL_HIGHWAY_SOUNDMANAGER_H