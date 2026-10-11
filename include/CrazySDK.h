#ifndef PIXEL_HIGHWAY_CRAZYSDK_H
#define PIXEL_HIGHWAY_CRAZYSDK_H

#include <functional>

// Small wrapper around the CrazyGames JavaScript SDK.
// On desktop every function is a harmless stub (ads "finish" instantly),
// so you can test the game flow in Visual Studio.
namespace CrazySDK
{
    enum class AdType
    {
        Midgame,
        Rewarded
    };

    // onDone(true)  -> the ad finished (give the reward if it was rewarded)
    // onDone(false) -> the ad failed or was not available
    // While an ad is running, IsAdPlaying() is true and sound is muted.
    void RequestAd(AdType type, std::function<void(bool completed)> onDone);
    bool IsAdPlaying();

    void GameplayStart();
    void GameplayStop();
    void LoadingFinished();
}

#endif //PIXEL_HIGHWAY_CRAZYSDK_H