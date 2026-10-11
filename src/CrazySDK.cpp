#include "../include/CrazySDK.h"

#include <utility>

#include "raylib.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

namespace
{
    bool g_adPlaying = false;
    std::function<void(bool)> g_onAdDone;

    void FinishAd(bool completed)
    {
        g_adPlaying = false;
        SetMasterVolume(1.0f);

        // Move the callback out first, so it can safely request another ad.
        std::function<void(bool)> callback = std::move(g_onAdDone);
        g_onAdDone = nullptr;

        if (callback) callback(completed);
    }
}

#if defined(__EMSCRIPTEN__)

// JavaScript calls these when the ad ends.
extern "C"
{
    EMSCRIPTEN_KEEPALIVE void CG_OnAdFinished() { FinishAd(true); }
    EMSCRIPTEN_KEEPALIVE void CG_OnAdError() { FinishAd(false); }
}

EM_JS(void, JS_RequestAd, (int rewarded), {
    if (!window.cgReady) {
        // SDK not usable here (other website, adblocker...). Report the error asynchronously.
        setTimeout(function() { Module.ccall("CG_OnAdError", null,[],[]); }, 0);
        return;
    }
    try {
        window.CrazyGames.SDK.ad.requestAd(rewarded ? "rewarded" : "midgame", {
            adStarted: function() {},
            adFinished : function() { Module.ccall("CG_OnAdFinished", null,[],[]); },
            adError : function(error) {
                console.log("CrazyGames ad error", error);
                Module.ccall("CG_OnAdError", null,[],[]);
            }
        });
    }
 catch (e) {
  console.log(e);
  setTimeout(function() { Module.ccall("CG_OnAdError", null,[],[]); }, 0);
}
    });

EM_JS(void, JS_GameplayStart, (), {
    try { if (window.cgReady) window.CrazyGames.SDK.game.gameplayStart(); }
 catch (e) {}
    });

EM_JS(void, JS_GameplayStop, (), {
    try { if (window.cgReady) window.CrazyGames.SDK.game.gameplayStop(); }
 catch (e) {}
    });

EM_JS(void, JS_LoadingFinished, (), {
    if (window.cgMarkLoaded) window.cgMarkLoaded();
    });

#endif

namespace CrazySDK
{
    void RequestAd(AdType type, std::function<void(bool completed)> onDone)
    {
        if (g_adPlaying) return; // one ad at a time

        g_adPlaying = true;
        g_onAdDone = std::move(onDone);
        SetMasterVolume(0.0f); // mute the game while the ad is running

#if defined(__EMSCRIPTEN__)
        JS_RequestAd(type == AdType::Rewarded ? 1 : 0);
#else
        (void)type;
        FinishAd(true); // desktop: pretend the ad finished
#endif
    }

    bool IsAdPlaying()
    {
        return g_adPlaying;
    }

    void GameplayStart()
    {
#if defined(__EMSCRIPTEN__)
        JS_GameplayStart();
#endif
    }

    void GameplayStop()
    {
#if defined(__EMSCRIPTEN__)
        JS_GameplayStop();
#endif
    }

    void LoadingFinished()
    {
#if defined(__EMSCRIPTEN__)
        JS_LoadingFinished();
#endif
    }
}