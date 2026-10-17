#include "../include/CarShop.h"

#include <fstream>

#include "../include/CoinManager.h"
#include "../include/TextUtils.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>

EM_JS(int, JS_LoadGarage, (int slot), {
    try {
        var keys = ["ph_car_owned", "ph_car_selected"];
        var value = localStorage.getItem(keys[slot]);
        if (value == null) return slot == 0 ? 1 : 0;
        return parseInt(value) || 0;
    }
  catch (e) { return slot == 0 ? 1 : 0; }
    });

EM_JS(void, JS_SaveGarage, (int slot, int v), {
    try {
        var keys = ["ph_car_owned", "ph_car_selected"];
        localStorage.setItem(keys[slot], v.toString());
    }
 catch (e) {}
    });
#else
namespace
{
    constexpr const char* GARAGE_FILE = "garage.txt"; // desktop only
}
#endif

namespace
{
    // The garage is laid out for the game's fixed 1280x720 size.
    // (Not read from GetScreenWidth(), because this class is created before the window exists.)
    constexpr float SCREEN_W = 1280.0f;
    constexpr float SCREEN_H = 720.0f;

    struct CarColor
    {
        const char* name;
        Color tint;
        int price;
    };

    // Colors that are NOT used by the traffic (blue, green, yellow, pink, grey, black).
    const std::array<CarColor, CarShop::ITEM_COUNT> ITEMS = { {
        { "Classic", Color{ 255, 255, 255, 255 }, 0 },
        { "Red",     Color{ 220,  30,  40, 255 }, 60 },
        { "Purple",  Color{ 150,  70, 220, 255 }, 120 },
        { "Orange",  Color{ 255, 105,   0, 255 }, 200 }
    } };

    void DrawCentered(const Font& font, const char* text, float centerX, float y,
        float size, float spacing, Color color)
    {
        const Vector2 textSize = MeasureTextEx(font, text, size, spacing);
        DrawTextOutlined(font, text, Vector2{ centerX - textSize.x / 2.0f, y }, size, spacing, color, BLACK);
    }

    // Coin icon + price, centered
    void DrawPrice(const Font& font, int price, float centerX, float y, Color color)
    {
        const char* text = TextFormat("%i", price);
        const Vector2 size = MeasureTextEx(font, text, 24, 3);

        const float iconSize = 26.0f;
        const float gap = 8.0f;
        const float left = centerX - (iconSize + gap + size.x) / 2.0f;

        CoinManager::DrawIcon(static_cast<int>(left + iconSize / 2.0f), static_cast<int>(y + size.y / 2.0f), 11.0f);
        DrawTextOutlined(font, text, Vector2{ left + iconSize + gap, y }, 24, 3, color, BLACK);
    }
}

CarShop::CarShop()
{
    m_ownedMask = LoadSlot(0);
    m_selected = LoadSlot(1);

    // Make sure the saved values make sense.
    m_ownedMask |= 1;
    m_ownedMask &= (1 << ITEM_COUNT) - 1;
    if (m_selected < 0 || m_selected >= ITEM_COUNT || !IsOwned(m_selected))
        m_selected = 0;

    // Layout: a row of cards, and a Back button below.
    const float cardWidth = 200.0f;
    const float cardHeight = 300.0f;
    const float gap = 24.0f;
    const float totalWidth = cardWidth * ITEM_COUNT + gap * (ITEM_COUNT - 1);
    const float startX = (SCREEN_W - totalWidth) / 2.0f;

    for (int i = 0; i < ITEM_COUNT; ++i)
        m_cards[i] = Rectangle{ startX + (cardWidth + gap) * static_cast<float>(i), 190.0f, cardWidth, cardHeight };

    m_backButton = Rectangle{ (SCREEN_W - 200.0f) / 2.0f, 600.0f, 200.0f, 50.0f };
}

CarShop::Event CarShop::Update(CoinManager& coins)
{
    if (m_messageTimer > 0.0f)
        m_messageTimer -= GetFrameTime();

    const Vector2 mouse = GetMousePosition();

    m_hovered = -1;
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        if (CheckCollisionPointRec(mouse, m_cards[i]))
            m_hovered = i;
    }
    m_hoverBack = CheckCollisionPointRec(mouse, m_backButton);

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return Event::None;

    if (m_hoverBack)
        return Event::Back;

    if (m_hovered < 0)
        return Event::None;

    const int index = m_hovered;

    // Owned: equip it.
    if (IsOwned(index))
    {
        if (m_selected == index)
            return Event::None;

        m_selected = index;
        Save();
        return Event::Equipped;
    }

    // Locked: try to buy it.
    if (!coins.TrySpend(ITEMS[index].price))
    {
        m_messageTimer = 1.5f;
        return Event::NotEnoughCoins;
    }

    m_ownedMask |= (1 << index);
    m_selected = index;
    Save();
    return Event::Purchased;
}

void CarShop::Draw(const Font& font, const Texture2D& carTexture, int totalCoins) const
{
    const float centerX = SCREEN_W / 2.0f;

    DrawCentered(font, "Garage", centerX, 40.0f, 44.0f, 5.0f, WHITE);

    // Total coins, centered under the title
    {
        const char* text = TextFormat("%i", totalCoins);
        const Vector2 size = MeasureTextEx(font, text, 30, 4);
        const float left = centerX - (26.0f + 12.0f + size.x) / 2.0f;

        CoinManager::DrawIcon(static_cast<int>(left + 13.0f), 128, 13.0f);
        DrawTextOutlined(font, text, Vector2{ left + 38.0f, 112.0f }, 30, 4, GOLD, BLACK);
    }

    // Cards
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        const Rectangle& card = m_cards[i];
        const CarColor& item = ITEMS[i];
        const bool selected = (i == m_selected);
        const bool owned = IsOwned(i);
        const bool affordable = totalCoins >= item.price;
        const float cardCenterX = card.x + card.width / 2.0f;

        DrawRectangleRec(card, i == m_hovered ? Color{ 60, 60, 75, 255 } : Color{ 40, 40, 50, 255 });
        DrawRectangleLinesEx(card, selected ? 4.0f : 2.0f, selected ? GOLD : Color{ 90, 90, 110, 255 });

        // The car, scaled up and tinted with this color
        const float previewHeight = 110.0f;
        const float scale = previewHeight / static_cast<float>(carTexture.height);
        const float previewWidth = static_cast<float>(carTexture.width) * scale;
        DrawTexturePro(carTexture,
            Rectangle{ 0.0f, 0.0f, static_cast<float>(carTexture.width), static_cast<float>(carTexture.height) },
            Rectangle{ cardCenterX - previewWidth / 2.0f, card.y + 30.0f, previewWidth, previewHeight },
            Vector2{ 0.0f, 0.0f }, 0.0f, item.tint);

        DrawCentered(font, item.name, cardCenterX, card.y + 165.0f, 26, 3, WHITE);

        if (selected)
            DrawCentered(font, "Equipped", cardCenterX, card.y + 210.0f, 22, 3, GOLD);
        else if (owned)
            DrawCentered(font, "Owned", cardCenterX, card.y + 210.0f, 22, 3, LIGHTGRAY);
        else
            DrawPrice(font, item.price, cardCenterX, card.y + 208.0f, affordable ? GOLD : RED);

        // Small hint
        const char* hint = "";
        if (!selected)
            hint = owned ? "Click to equip" : (affordable ? "Click to buy" : "Need more coins");
        DrawCentered(font, hint, cardCenterX, card.y + 255.0f, 16, 2, LIGHTGRAY);
    }

    if (m_messageTimer > 0.0f)
        DrawCentered(font, "Not enough coins", centerX, 520.0f, 28, 3, RED);

    // Back button (same look as the difficulty buttons)
    DrawRectangleRounded(m_backButton, 0.7f, 8, m_hoverBack ? ORANGE : YELLOW);
    const Vector2 backSize = MeasureTextEx(font, "Back", 22, 3);
    DrawTextEx(font, "Back",
        Vector2{ m_backButton.x + (m_backButton.width - backSize.x) / 2.0f,
                 m_backButton.y + (m_backButton.height - backSize.y) / 2.0f },
        22, 3, m_hoverBack ? BLACK : RED);
}

Color CarShop::GetSelectedTint() const
{
    return ITEMS[m_selected].tint;
}

bool CarShop::IsOwned(int index) const
{
    return ((m_ownedMask >> index) & 1) != 0;
}

int CarShop::LoadSlot(int slot)
{
#if defined(__EMSCRIPTEN__)
    return JS_LoadGarage(slot);
#else
    int owned = 1;
    int selected = 0;

    std::ifstream file(GARAGE_FILE);
    if (file.is_open())
        file >> owned >> selected;

    return slot == 0 ? owned : selected;
#endif
}

void CarShop::Save() const
{
#if defined(__EMSCRIPTEN__)
    JS_SaveGarage(0, m_ownedMask);
    JS_SaveGarage(1, m_selected);
#else
    std::ofstream file(GARAGE_FILE);
    if (file.is_open())
        file << m_ownedMask << " " << m_selected;
#endif
}