#include "../include/PowerUpManager.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "../include/TextUtils.h"

namespace
{
    float RandomRange(float low, float high)
    {
        if (high < low) std::swap(low, high);
        return low + (high - low) * (GetRandomValue(0, 1000) / 1000.0f);
    }
}

void PowerUpManager::ResetRun()
{
    m_pickups.clear();
    m_spawnTimer = FIRST_SPAWN_TIME * m_spawnScale;

    m_shieldCharges = 0;
    m_magnetTimer = 0.0f;
    m_slowTimer = 0.0f;
    m_slowBlend = 0.0f;
}

void PowerUpManager::SetUpgrades(float magnetDuration, float magnetRadius, float slowDuration,
    float spawnTimeScale, int shieldCharges)
{
    m_magnetDuration = magnetDuration;
    m_magnetRadius = magnetRadius;
    m_slowDuration = slowDuration;
    m_spawnScale = std::max(0.3f, spawnTimeScale);
    m_shieldMaxCharges = std::max(1, shieldCharges);
}

void PowerUpManager::Update(float worldMove, float frameTime, float roadX, float roadWidth,
    const std::vector<Rectangle>& traffic)
{
    m_time += frameTime;

    if (m_magnetTimer > 0.0f)
        m_magnetTimer = std::max(0.0f, m_magnetTimer - frameTime);

    if (m_slowTimer > 0.0f)
        m_slowTimer = std::max(0.0f, m_slowTimer - frameTime);

    // The slow motion eases in and out, so the world never changes speed in a single frame.
    const float target = m_slowTimer > 0.0f ? 1.0f : 0.0f;
    const float blendStep = SLOW_BLEND_RATE * frameTime;
    m_slowBlend = m_slowBlend < target ? std::min(target, m_slowBlend + blendStep)
        : std::max(target, m_slowBlend - blendStep);

    // Pickups move with the world, exactly like the traffic.
    for (auto& pickup : m_pickups)
        pickup.position.y += worldMove;

    const float bottom = static_cast<float>(GetScreenHeight()) + DRAWN_RADIUS * 2.0f;
    std::erase_if(m_pickups, [bottom](const Pickup& pickup) { return pickup.position.y > bottom; });

    m_spawnTimer -= frameTime;
    if (m_spawnTimer <= 0.0f)
    {
        // If there was no free spot, try again in a second.
        m_spawnTimer = TrySpawn(roadX, roadWidth, traffic)
            ? RandomRange(SPAWN_TIME_MIN, SPAWN_TIME_MAX) * m_spawnScale
            : 1.0f;
    }
}

bool PowerUpManager::TrySpawn(float roadX, float roadWidth, const std::vector<Rectangle>& traffic)
{
    // Only offer a power the player does not have in full, and that is not already lying on the road.
    std::vector<Type> options;

    const auto onRoad = [this](Type type)
    {
        return std::any_of(m_pickups.begin(), m_pickups.end(),
            [type](const Pickup& pickup) { return pickup.type == type; });
    };

    if (m_shieldCharges < m_shieldMaxCharges && !onRoad(Type::Shield)) options.push_back(Type::Shield);
    if (m_magnetTimer <= 0.0f && !onRoad(Type::Magnet)) options.push_back(Type::Magnet);
    if (m_slowTimer <= 0.0f && !onRoad(Type::SlowMotion)) options.push_back(Type::SlowMotion);

    if (options.empty())
        return true; // nothing useful to give right now: wait for the next timer

    const Type type = options[GetRandomValue(0, static_cast<int>(options.size()) - 1)];

    const float minX = roadX + SPAWN_EDGE_MARGIN;
    const float maxX = roadX + roadWidth - SPAWN_EDGE_MARGIN;
    if (maxX <= minX) return false;

    const float y = -DRAWN_RADIUS * 2.0f;
    const float half = DRAWN_RADIUS + TRAFFIC_CLEARANCE;

    // Try a few random positions and pick one that is not on top of a traffic car.
    for (int attempt = 0; attempt < 10; ++attempt)
    {
        const float x = RandomRange(minX, maxX);
        const Rectangle area{ x - half, y - half, half * 2.0f, half * 2.0f };

        const bool blocked = std::any_of(traffic.begin(), traffic.end(),
            [&area](const Rectangle& car) { return CheckCollisionRecs(area, car); });
        if (blocked) continue;

        m_pickups.push_back(Pickup{ type, Vector2{ x, y }, RandomRange(0.0f, 6.0f) });
        return true;
    }

    return false;
}

std::vector<PowerUpManager::Type> PowerUpManager::CollectTouching(const Rectangle& carRect)
{
    std::vector<Type> collected;

    for (auto it = m_pickups.begin(); it != m_pickups.end();)
    {
        if (CheckCollisionCircleRec(it->position, COLLECT_RADIUS, carRect))
        {
            const Type type = it->type;
            it = m_pickups.erase(it);

            switch (type)
            {
            case Type::Shield:     m_shieldCharges = m_shieldMaxCharges; break; // a full shield
            case Type::Magnet:     m_magnetTimer = m_magnetDuration; break;
            case Type::SlowMotion: m_slowTimer = m_slowDuration; break;
            }

            collected.push_back(type);
        }
        else
        {
            ++it;
        }
    }

    return collected;
}

bool PowerUpManager::ConsumeShield()
{
    if (m_shieldCharges <= 0) return false;

    --m_shieldCharges;
    return true;
}

bool PowerUpManager::HasShield() const
{
    return m_shieldCharges > 0;
}

bool PowerUpManager::IsMagnetActive() const
{
    return m_magnetTimer > 0.0f;
}

bool PowerUpManager::IsSlowMotionActive() const
{
    return m_slowTimer > 0.0f;
}

float PowerUpManager::GetMagnetRadius() const
{
    return m_magnetRadius;
}

float PowerUpManager::GetTimeScale() const
{
    return 1.0f - (1.0f - SLOW_SCALE) * m_slowBlend;
}

void PowerUpManager::DrawPickups(const Font& font) const
{
    for (const auto& pickup : m_pickups)
    {
        const float bob = 1.0f + 0.08f * std::sin(m_time * 5.0f + pickup.phase);
        const float radius = DRAWN_RADIUS * bob;

        // Soft glow, then the icon
        DrawCircleV(pickup.position, radius + 9.0f, ColorAlpha(GetColor(pickup.type), 0.25f));
        DrawIcon(font, pickup.position, radius, pickup.type, 1.0f);
    }
}

void PowerUpManager::DrawCarEffects(const Rectangle& carRect, bool invincible) const
{
    const Vector2 center{ carRect.x + carRect.width / 2.0f, carRect.y + carRect.height / 2.0f };

    // Magnet: rings that shrink toward the car, like coins being pulled in.
    if (IsMagnetActive())
    {
        const Color color = GetColor(Type::Magnet);
        const float span = m_magnetRadius - 30.0f;

        for (int i = 0; i < 2; ++i)
        {
            const float progress = std::fmod(m_time * 0.9f + 0.5f * static_cast<float>(i), 1.0f); // 0 -> 1
            const float radius = m_magnetRadius - span * progress;

            DrawRing(center, radius - 2.0f, radius, 0.0f, 360.0f, 48, ColorAlpha(color, 0.15f + 0.45f * progress));
        }
    }

    if (HasShield())
    {
        // Shield: a pulsing bubble around the car, with one ring for each hit it can still absorb.
        const Color color = GetColor(Type::Shield);
        const float pulse = 1.0f + 0.05f * std::sin(m_time * 6.0f);
        const float radius = (std::max(carRect.width, carRect.height) * 0.5f + 16.0f) * pulse;

        DrawCircleV(center, radius, ColorAlpha(color, 0.18f));

        for (int i = 0; i < m_shieldCharges; ++i)
        {
            const float ringRadius = radius + 6.0f * static_cast<float>(i);
            DrawRing(center, ringRadius - 3.0f, ringRadius, 0.0f, 360.0f, 36, ColorAlpha(color, 0.9f));
        }
    }
    else if (invincible)
    {
        // Short protection (after Continue or a broken shield): a flickering white ring.
        if (std::fmod(m_time * 10.0f, 2.0f) < 1.0f)
        {
            const float radius = std::max(carRect.width, carRect.height) * 0.5f + 14.0f;
            DrawRing(center, radius - 2.0f, radius, 0.0f, 360.0f, 36, ColorAlpha(WHITE, 0.8f));
        }
    }
}

void PowerUpManager::DrawScreenEffects() const
{
    if (m_slowBlend <= 0.01f) return;

    // Slow motion: a soft tint over the whole screen, and a border.
    const Color color = GetColor(Type::SlowMotion);
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();

    DrawRectangle(0, 0, width, height, ColorAlpha(color, 0.10f * m_slowBlend));
    DrawRectangleLinesEx(Rectangle{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) },
        10.0f, ColorAlpha(color, 0.35f * m_slowBlend));
}

void PowerUpManager::DrawHud(const Font& font, float x, float y) const
{
    const float radius = 18.0f;
    const float slot = 56.0f; // fixed slots, so the icons never jump around

    // Slot 0: the shield
    if (HasShield())
    {
        DrawIcon(font, Vector2{ x + radius, y + radius }, radius, Type::Shield, 1.0f);

        // With the upgrade, show how many hits are left
        if (m_shieldMaxCharges > 1)
        {
            DrawTextOutlined(font, TextFormat("x%i", m_shieldCharges),
                Vector2{ x + radius * 2.0f - 8.0f, y + radius + 2.0f }, 18, 2, WHITE, BLACK);
        }
    }

    // Slots 1 and 2: powers with a timer (icon + a bar that runs out)
    const auto drawTimed = [&](int slotIndex, Type type, float timer, float duration)
    {
        const float centerX = x + slot * static_cast<float>(slotIndex) + radius;
        const float fraction = std::clamp(timer / duration, 0.0f, 1.0f);

        // Blink during the last two seconds
        const bool dim = timer < 2.0f && std::fmod(m_time * 8.0f, 2.0f) < 1.0f;
        DrawIcon(font, Vector2{ centerX, y + radius }, radius, type, dim ? 0.35f : 1.0f);

        const float barY = y + radius * 2.0f + 8.0f;
        DrawRectangle(static_cast<int>(centerX - radius), static_cast<int>(barY),
            static_cast<int>(radius * 2.0f), 6, ColorAlpha(BLACK, 0.5f));
        DrawRectangle(static_cast<int>(centerX - radius), static_cast<int>(barY),
            static_cast<int>(radius * 2.0f * fraction), 6, GetColor(type));
    };

    if (IsMagnetActive())
        drawTimed(1, Type::Magnet, m_magnetTimer, m_magnetDuration);

    if (IsSlowMotionActive())
        drawTimed(2, Type::SlowMotion, m_slowTimer, m_slowDuration);
}

void PowerUpManager::DrawIcon(const Font& font, Vector2 center, float radius, Type type, float alpha)
{
    DrawCircleV(center, radius + 3.0f, ColorAlpha(BLACK, alpha));
    DrawCircleV(center, radius, ColorAlpha(GetColor(type), alpha));

    // Black letter: all the colors are bright, so black is the most readable
    const char* letter = GetLetter(type);
    const float fontSize = radius * 1.5f;
    const Vector2 size = MeasureTextEx(font, letter, fontSize, 1.0f);
    DrawTextEx(font, letter, Vector2{ center.x - size.x / 2.0f, center.y - size.y / 2.0f },
        fontSize, 1.0f, ColorAlpha(BLACK, alpha));
}

const char* PowerUpManager::GetName(Type type)
{
    switch (type)
    {
    case Type::Shield:     return "SHIELD!";
    case Type::Magnet:     return "MAGNET!";
    case Type::SlowMotion: return "SLOW MOTION!";
    }
    return "";
}

const char* PowerUpManager::GetLetter(Type type)
{
    switch (type)
    {
    case Type::Shield:     return "S";
    case Type::Magnet:     return "M";
    case Type::SlowMotion: return "T"; // T for time
    }
    return "";
}

Color PowerUpManager::GetColor(Type type)
{
    // Not gold (coins), and not close to the traffic colors.
    switch (type)
    {
    case Type::Shield:     return Color{ 0, 230, 255, 255 };   // cyan
    case Type::Magnet:     return Color{ 255, 60, 200, 255 };  // magenta
    case Type::SlowMotion: return Color{ 170, 130, 255, 255 }; // lavender
    }
    return WHITE;
}