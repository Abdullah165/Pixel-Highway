#include "../include/CoinManager.h"
#include "../include/TextUtils.h"
#include "../include/Steering.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <utility>


#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>

EM_JS(int, JS_LoadCoins, (), {
    try { return parseInt(localStorage.getItem("ph_coins") || "0") || 0; }
 catch (e) { return 0; }
    });

EM_JS(void, JS_SaveCoins, (int v), {
    try { localStorage.setItem("ph_coins", v.toString()); }
 catch (e) {}
    });
#else
namespace
{
    constexpr const char* COINS_FILE = "coins.txt"; // desktop only
}
#endif

namespace
{
    float RandomRange(float low, float high)
    {
        if (high < low) std::swap(low, high);
        return low + (high - low) * (GetRandomValue(0, 1000) / 1000.0f);
    }
}

CoinManager::CoinManager()
{
    m_totalCoins = LoadTotal();
}

void CoinManager::ResetRun()
{
    m_coins.clear();
    m_runCoins = 0;
    m_chain = 0;
    m_chainTimer = 0.0f;
    m_hudPulse = 0.0f;
    m_distanceUntilSpawn = FIRST_SPAWN_DISTANCE;
    m_doubledBase = 0;
}

void CoinManager::Update(float worldMove, float frameTime, float roadX, float roadWidth, float worldSpeed)
{
    m_time += frameTime;

    // The chain (used for the rising pitch) ends if no coin is collected for a while.
    if (m_chainTimer > 0.0f)
    {
        m_chainTimer -= frameTime;
        if (m_chainTimer <= 0.0f)
            m_chain = 0;
    }

    if (m_hudPulse > 0.0f)
        m_hudPulse = std::max(0.0f, m_hudPulse - frameTime);

    // Coins move with the world, exactly like the traffic.
    for (auto& coin : m_coins)
        coin.position.y += worldMove;

    const float bottom = static_cast<float>(GetScreenHeight()) + RADIUS * 2.0f;
    std::erase_if(m_coins, [bottom](const Coin& coin) { return coin.position.y > bottom; });

    m_distanceUntilSpawn -= worldMove;
    if (m_distanceUntilSpawn <= 0.0f)
    {
        SpawnPattern(roadX, roadWidth, worldSpeed);
        m_distanceUntilSpawn = RandomRange(SPAWN_GAP_MIN, SPAWN_GAP_MAX);
    }
}

void CoinManager::SpawnPattern(float roadX, float roadWidth, float worldSpeed)
{
    const float minX = roadX + EDGE_MARGIN;
    const float maxX = roadX + roadWidth - EDGE_MARGIN;
    if (maxX <= minX) return;

    const float span = maxX - minX;

    // Time-based spacing: the same time between two coins at any world speed.
    const float spacing = std::max(MIN_COIN_SPACING, worldSpeed * FRAMES_BETWEEN_COINS);

    // How far the player can comfortably steer between two coins.
    const float comfortableStep = Steering::SpeedFor(worldSpeed) * FRAMES_BETWEEN_COINS * STEER_EFFORT;

    const int pattern = GetRandomValue(0, 2); // 0 = line, 1 = diagonal, 2 = wave

    // Line
    const float lineX = RandomRange(minX, maxX);

    // Diagonal
    const float diagonalStep = std::min(comfortableStep, span / (PATTERN_COUNT - 1));
    const float diagonalShift = diagonalStep * (PATTERN_COUNT - 1);
    const bool goesRight = GetRandomValue(0, 1) == 0;
    const float diagonalStart = goesRight ? RandomRange(minX, maxX - diagonalShift)
        : RandomRange(minX + diagonalShift, maxX);

    // Wave
    const float amplitude = std::min(WAVE_AMPLITUDE, span / 2.0f);
    const float waveCenter = RandomRange(minX + amplitude, maxX - amplitude);

    for (int i = 0; i < PATTERN_COUNT; ++i)
    {
        float x = lineX;

        if (pattern == 1)
            x = diagonalStart + (goesRight ? 1.0f : -1.0f) * diagonalStep * static_cast<float>(i);
        else if (pattern == 2)
            x = waveCenter + amplitude * std::sin(static_cast<float>(i) * PI / 2.0f);

        // Above the screen: the first coin arrives first, the others follow in a column.
        const float y = -RADIUS - spacing * static_cast<float>(i);

        m_coins.push_back(Coin{ Vector2{x, y}, RandomRange(0.0f, 6.0f) });
    }
}

void CoinManager::RemoveOverlapping(const Rectangle& rect)
{
    std::erase_if(m_coins, [&rect](const Coin& coin)
    {
        return CheckCollisionCircleRec(coin.position, RADIUS, rect);
    });
}

std::vector<Vector2> CoinManager::CollectTouching(const Rectangle& carRect)
{
    std::vector<Vector2> collected;

    for (auto it = m_coins.begin(); it != m_coins.end();)
    {
        if (CheckCollisionCircleRec(it->position, PICKUP_RADIUS, carRect))
        {
            collected.push_back(it->position);
            it = m_coins.erase(it);

            ++m_runCoins;
            ++m_totalCoins;
            m_totalChanged = true;

            ++m_chain;
            m_chainTimer = CHAIN_TIME;
            m_hudPulse = PULSE_TIME;
        }
        else
        {
            ++it;
        }
    }

    return collected;
}

void CoinManager::Draw() const
{
    for (const auto& coin : m_coins)
        DrawCoinShape(coin.position, coin.phase);
}

void CoinManager::DrawCoinShape(Vector2 center, float phase) const
{
    // The coin "spins": its width grows and shrinks like a coin turning around.
    const float spin = std::max(0.25f, std::fabs(std::cos(m_time * 5.0f + phase)));
    const float halfWidth = RADIUS * spin;

    const int x = static_cast<int>(center.x);
    const int y = static_cast<int>(center.y);

    DrawEllipse(x, y, halfWidth + 2.0f, RADIUS + 2.0f, BLACK);        // outline
    DrawEllipse(x, y, halfWidth, RADIUS, GOLD);                       // body
    DrawEllipse(x, y, halfWidth * 0.55f, RADIUS * 0.6f, YELLOW);      // lighter center
}

void CoinManager::DrawHud(const Font& font, float x, float y) const
{
    // Static coin icon
    const int iconX = static_cast<int>(x) + 14;
    const int iconY = static_cast<int>(y) + 14;
    DrawCircle(iconX, iconY, 14.0f, BLACK);
    DrawCircle(iconX, iconY, 12.0f, GOLD);
    DrawCircle(iconX, iconY, 7.0f, YELLOW);

    // The number "pops" for a moment when a coin is collected.
    const float pulse = m_hudPulse / PULSE_TIME; // 1 -> 0
    const float fontSize = 28.0f + 8.0f * pulse;

    DrawTextOutlined(font, TextFormat("%i", m_runCoins),
        Vector2{ x + 36.0f, y - -2.0f }, fontSize, 4, GOLD, BLACK);
}

float CoinManager::GetPickupPitch() const
{
    // Higher than the close-call sound, and rising with the chain.
    return 1.5f + 0.06f * static_cast<float>(std::clamp(m_chain - 1, 0, 8));
}

int CoinManager::GetRunCoins() const
{
    return m_runCoins;
}

int CoinManager::GetTotalCoins() const
{
    return m_totalCoins;
}

void CoinManager::Save()
{
    if (!m_totalChanged) return;

    WriteTotal(m_totalCoins);
    m_totalChanged = false;
}

int CoinManager::GetDoubleableCoins() const
{
    return m_runCoins - m_doubledBase;
}

void CoinManager::DoubleRunCoins()
{
    const int bonus = GetDoubleableCoins();
    if (bonus <= 0) return;

    m_runCoins += bonus;
    m_totalCoins += bonus;
    m_doubledBase = m_runCoins; // everything collected so far is now accounted for
    m_totalChanged = true;

    Save(); // saved right away: the player just earned it
}

int CoinManager::LoadTotal() const
{
#if defined(__EMSCRIPTEN__)
    return JS_LoadCoins();
#else
    std::ifstream file(COINS_FILE);
    int total = 0;
    if (file.is_open())
        file >> total;
    return total;
#endif
}

void CoinManager::WriteTotal(int total) const
{
#if defined(__EMSCRIPTEN__)
    JS_SaveCoins(total);
#else
    std::ofstream file(COINS_FILE);
    if (file.is_open())
        file << total;
#endif
}