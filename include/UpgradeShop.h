#ifndef PIXEL_HIGHWAY_UPGRADESHOP_H
#define PIXEL_HIGHWAY_UPGRADESHOP_H

#include <array>

#include "raylib.h"

class CoinManager;

// The "Upgrades" tab of the garage: permanent upgrades for the power-ups, bought with coins.
class UpgradeShop
{
public:
    static constexpr int UPGRADE_COUNT = 5;

    enum class Event
    {
        None,
        Back,
        Purchased,
        NotEnoughCoins
    };

    UpgradeShop();

    // Handles hover and clicks. Call once per frame while this tab is open.
    Event Update(CoinManager& coins);

    // Draws the tab (the caller clears the background and draws the tab buttons).
    void Draw(const Font& font, int totalCoins) const;

    // Values the game uses. They depend on the levels bought here.
    float GetMagnetDuration() const;
    float GetMagnetRadius() const;
    float GetSpawnTimeScale() const; // below 1.0 = power-ups appear more often
    int GetShieldCharges() const;    // hits a shield can absorb
    float GetSlowDuration() const;

private:
    static int LoadPacked();
    void Save() const;

    std::array<int, UPGRADE_COUNT> m_levels{};
    std::array<Rectangle, UPGRADE_COUNT> m_cards{};
    Rectangle m_backButton{};

    int m_hovered = -1;
    bool m_hoverBack = false;
    float m_messageTimer = 0.0f; // shows "Not enough coins" for a moment
};

#endif //PIXEL_HIGHWAY_UPGRADESHOP_H