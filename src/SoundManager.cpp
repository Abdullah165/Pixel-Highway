#include "../include/SoundManager.h"

void SoundManager::InitResources()
{
    if (IsAudioDeviceReady()) return; // already started

    InitAudioDevice();
    m_sounds["Racing"] = LoadSound("assets/sounds/Music/Racing.mp3");
    m_sounds["CarCrash"] = LoadSound("assets/sounds/SFX/CarCrash.mp3");
    m_sounds["CarEngine"] = LoadSound("assets/sounds/SFX/CarEngine.mp3");
    m_sounds["Traffic"] = LoadSound("assets/sounds/SFX/Traffic.mp3");
    m_sounds["ButtonClick"] = LoadSound("assets/sounds/SFX/ButtonClick.mp3");
    m_sounds["CloseCall"] = LoadSound("assets/sounds/SFX/CloseCall.mp3");
    m_sounds["Coin"] = LoadSound("assets/sounds/SFX/CloseCall.mp3"); // same file as CloseCall, played at a higher pitch
  
    SetSoundVolume(m_sounds["Coin"], 0.5f); // coins are frequent, so keep them quieter

    SetSoundVolume(m_sounds["CloseCall"], 0.7f); // lower it if it is louder than your other sounds
}

void SoundManager::StartPlayingSound(const std::string& key)
{
    const auto it = m_sounds.find(key);
    if (it != m_sounds.end())
        PlaySound(it->second);
}

void SoundManager::StopPlayingSound(const std::string& key)
{
    const auto it = m_sounds.find(key);
    if (it != m_sounds.end())
        StopSound(it->second);
}

void SoundManager::PlaySoundWithPitch(const std::string& key, float pitch)
{
    const auto it = m_sounds.find(key);
    if (it == m_sounds.end()) return;

    SetSoundPitch(it->second, pitch); // 1.0 = normal, higher = higher and faster
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
void SoundManager::SetPitch(const std::string& key, float pitch)
{
    const auto it = m_sounds.find(key);
    if (it != m_sounds.end())
        SetSoundPitch(it->second, pitch);
}