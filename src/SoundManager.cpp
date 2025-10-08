#include "../include/SoundManager.h"

void SoundManager::InitResources()
{
    InitAudioDevice();
    m_sounds["Racing"] = LoadSound("assets/sounds/Music/Racing.mp3");
    m_sounds["CarEngine"] = LoadSound("assets/sounds/SFX/CarEngine.mp3");
    m_sounds["CarCrash"] = LoadSound("assets/sounds/SFX/CarCrash.mp3");
}

void SoundManager::StartPlayingSound(const std::string& key)
{
    PlaySound( m_sounds[key]);
}

void SoundManager::ShutdownAllSounds()
{
    for (auto&[key,sound] : m_sounds)
    {
        UnloadSound(sound);
    }

    m_sounds.clear();
}

bool SoundManager::IsSoundStillPlaying(const std::string& key)
{
    return IsSoundPlaying(m_sounds[key]);
}