#ifndef PIXEL_HIGHWAY_CARSHOP_H
#define PIXEL_HIGHWAY_CARSHOP_H

#include <array>

#include "raylib.h"

class CoinManager;

// The garage: car colors you can buy with coins and equip.
class CarShop
{
public:
    static constexpr int ITEM_COUNT = 4;

    enum class Event
    {
        None,
        Back,
        Equipped,
        Purchased,
        NotEnoughCoins
    };

    CarShop();

    // Handles hover and clicks. Call once per frame while the garage is open.
    Event Update(CoinManager& coins);

    // Draws the garage (the caller clears the background).
    void Draw(const Font& font, const Texture2D& carTexture, int totalCoins) const;

    Color GetSelectedTint() const;

private:
    bool IsOwned(int index) const;
    static int LoadSlot(int slot); // 0 = owned colors, 1 = equipped color
    void Save() const;

    int m_ownedMask = 1; // bit i = color i is owned (bit 0 = the free Classic color)
    int m_selected = 0;

    std::array<Rectangle, ITEM_COUNT> m_cards{};
    Rectangle m_backButton{};

    int m_hovered = -1;
    bool m_hoverBack = false;
    float m_messageTimer = 0.0f; // shows "Not enough coins" for a moment
};

#endif //PIXEL_HIGHWAY_CARSHOP_H