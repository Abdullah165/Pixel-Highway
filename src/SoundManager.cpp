#include "../include/SoundManager.h"

void SoundManager::InitResources()
{
    if (IsAudioDeviceReady()) return; // already started

    InitAudioDevice();
    m_sounds["Racing"] = LoadSound("assets/sounds/Music/Racing.mp3");
    m_sounds["CarCrash"] = LoadSound("assets/sounds/SFX/CarCrash.mp3");
    m_sounds["Traffic"] = LoadSound("assets/sounds/SFX/Traffic.mp3");
    m_sounds["ButtonClick"] = LoadSound("assets/sounds/SFX/ButtonClick.mp3");
}

void SoundManager::StartPlayingSound(const std::string& key)
{
    const auto it = m_sounds.find(key);
    if (it != m_sounds.end())
        PlaySound(it->second);
}

bool SoundManager::IsSoundStillPlaying(const std::string& key) const
{
    const auto it = m_sounds.find(key);
    return it != m_sounds.end() && IsSoundPlaying(it->second);
}

void SoundManager::ShutdownAllSounds()
{
    for (auto& [key, sound] : m_sounds)
    {
        UnloadSound(sound);
    }
    m_sounds.clear();

    if (IsAudioDeviceReady())
        CloseAudioDevice();
}