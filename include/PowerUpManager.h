#ifndef PIXEL_HIGHWAY_POWERUPMANAGER_H
#define PIXEL_HIGHWAY_POWERUPMANAGER_H

#include <vector>

#include "raylib.h"

// Power-up pickups on the road (Shield, Magnet, Slow Motion) and their active effects.
class PowerUpManager
{
public:
    enum class Type
    {
        Shield,
        Magnet,
        SlowMotion
    };

    void ResetRun(); // new run: clear pickups and effects (the upgrade values stay)

    // Called before each run with the values from the upgrade shop.
    void SetUpgrades(float magnetDuration, float magnetRadius, float slowDuration,
        float spawnTimeScale, int shieldCharges);

    // worldMove = how far the world moved this frame. traffic = rectangles of the NPC cars.
    void Update(float worldMove, float frameTime, float roadX, float roadWidth,
        const std::vector<Rectangle>& traffic);

    // Collects every pickup touching the car and activates it. Returns what was collected.
    std::vector<Type> CollectTouching(const Rectangle& carRect);

    bool ConsumeShield(); // true if a shield absorbed the hit

    bool HasShield() const;
    bool IsMagnetActive() const;
    bool IsSlowMotionActive() const;
    float GetMagnetRadius() const;

    // 1.0 = normal. Lower = slow motion. It eases in and out, so it never jumps.
    float GetTimeScale() const;

    void DrawPickups(const Font& font) const;                              // on the road, under the cars
    void DrawCarEffects(const Rectangle& carRect, bool invincible) const;  // around the player's car
    void DrawScreenEffects() const;                                        // full-screen tint (slow motion)
    void DrawHud(const Font& font, float x, float y) const;                // active power icons

    static void DrawIcon(const Font& font, Vector2 center, float radius, Type type, float alpha);
    static const char* GetName(Type type);
    static Color GetColor(Type type);

private:
    // ---- Tuning (the defaults, before any upgrade) ----
    static constexpr float DRAWN_RADIUS = 18.0f;
    static constexpr float COLLECT_RADIUS = 26.0f;       // collected a bit before the car really touches it
    static constexpr float FIRST_SPAWN_TIME = 12.0f;     // seconds until the first pickup
    static constexpr float SPAWN_TIME_MIN = 18.0f;       // seconds between pickups
    static constexpr float SPAWN_TIME_MAX = 30.0f;
    static constexpr float SPAWN_EDGE_MARGIN = 46.0f;    // keeps pickups inside the road
    static constexpr float TRAFFIC_CLEARANCE = 30.0f;    // free space around a new pickup
    static constexpr float MAGNET_DURATION = 8.0f;       // seconds
    static constexpr float MAGNET_RADIUS = 220.0f;       // pixels
    static constexpr float SLOW_DURATION = 5.0f;         // seconds
    static constexpr float SLOW_SCALE = 0.7f;            // the world runs at 60% speed
    static constexpr float SLOW_BLEND_RATE = 5.0f;       // ease in/out: full effect in 0.2 seconds

    struct Pickup
    {
        Type type;
        Vector2 position;
        float phase; // makes the bobbing out of sync
    };

    bool TrySpawn(float roadX, float roadWidth, const std::vector<Rectangle>& traffic);
    static const char* GetLetter(Type type);

    std::vector<Pickup> m_pickups;
    float m_spawnTimer = FIRST_SPAWN_TIME;
    float m_time = 0.0f;

    int m_shieldCharges = 0;      // hits the shield can still absorb
    float m_magnetTimer = 0.0f;
    float m_slowTimer = 0.0f;
    float m_slowBlend = 0.0f;     // 0 = normal speed, 1 = full slow motion

    // Set by the upgrade shop before each run.
    int m_shieldMaxCharges = 1;
    float m_magnetDuration = MAGNET_DURATION;
    float m_magnetRadius = MAGNET_RADIUS;
    float m_slowDuration = SLOW_DURATION;
    float m_spawnScale = 1.0f;    // below 1.0 = power-ups appear more often
};

#endif //PIXEL_HIGHWAY_POWERUPMANAGER_H