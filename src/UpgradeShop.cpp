#include "../include/UpgradeShop.h"

#include <algorithm>
#include <fstream>
#include <string>

#include "../include/CoinManager.h"
#include "../include/PowerUpManager.h"
#include "../include/TextUtils.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>

EM_JS(int, JS_LoadUpgrades, (), {
	try { return parseInt(localStorage.getItem("ph_upgrades") || "0") || 0; }
 catch (e) { return 0; }
	});

EM_JS(void, JS_SaveUpgrades, (int v), {
	try { localStorage.setItem("ph_upgrades", v.toString()); }
 catch (e) {}
	});
#else
namespace
{
	constexpr const char* UPGRADES_FILE = "upgrades.txt"; // desktop only
}
#endif

namespace
{
	// Laid out for the game's fixed 1280x720 size (same as the car shop).
	constexpr float SCREEN_W = 1280.0f;

	constexpr int MAGNET_TIME = 0;
	constexpr int MAGNET_RANGE = 1;
	constexpr int LUCKY_DROPS = 2;
	constexpr int SHIELD_PLUS = 3;
	constexpr int SLOW_TIME = 4;

	struct UpgradeInfo
	{
		const char* name;
		const char* description;
		int maxLevel;
		std::array<int, 3> prices; // price of level 1, 2, 3
	};

	const std::array<UpgradeInfo, UpgradeShop::UPGRADE_COUNT> INFO = { {
		{ "Magnet Time",  "Longer magnet",  3, { 80, 200, 400 } },
		{ "Magnet Range", "Wider pull",     3, { 80, 200, 400 } },
		{ "Lucky Drops",  "More power-ups", 3, { 80, 200, 400 } },
		{ "Shield Plus",  "Tougher shield", 1, { 300, 0, 0 } },
		{ "Slow Time",    "Longer slow-mo", 3, { 100, 250, 500 } }

	} };

	// ---- What each level gives. Level 0 = the default values of PowerUpManager. ----
	float MagnetTimeAt(int level) { return 8.0f + 2.0f * static_cast<float>(level); }
	float MagnetRangeAt(int level) { return 220.0f + 40.0f * static_cast<float>(level); }
	float SpawnScaleAt(int level) { return 1.0f - 0.12f * static_cast<float>(level); } // 1.0, 0.85, 0.70, 0.55
	int ShieldHitsAt(int level) { return 1 + level; }
	float SlowTimeAt(int level) { return 5.0f + 1.5f * static_cast<float>(level); } // 5, 6.5, 8, 9.5 seconds

	// The text shown on a card for a level, for example "8s" or "2 hits".
	std::string ValueText(int id, int level)
	{
		switch (id)
		{
		case MAGNET_TIME:  return TextFormat("%.0fs", MagnetTimeAt(level));
		case MAGNET_RANGE: return TextFormat("%.0f", MagnetRangeAt(level));
		case LUCKY_DROPS:  return TextFormat("~%.0fs", 24.0f * SpawnScaleAt(level)); // 24 s = average spawn time
		case SHIELD_PLUS:
		{
			const int hits = ShieldHitsAt(level);
			return TextFormat(hits == 1 ? "%i hit" : "%i hits", hits);
		}
		case SLOW_TIME:    return TextFormat("%.1fs", SlowTimeAt(level));
		}
		return "";
	}

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

	void DrawUpgradeIcon(const Font& font, int id, Vector2 center, float radius)
	{
		switch (id)
		{
		case MAGNET_TIME:
		case MAGNET_RANGE:
			PowerUpManager::DrawIcon(font, center, radius, PowerUpManager::Type::Magnet, 1.0f);
			break;
		case SHIELD_PLUS:
			PowerUpManager::DrawIcon(font, center, radius, PowerUpManager::Type::Shield, 1.0f);
			break;
		case SLOW_TIME:
			PowerUpManager::DrawIcon(font, center, radius, PowerUpManager::Type::SlowMotion, 1.0f);
			break;
		case LUCKY_DROPS:
		{
			DrawCircleV(center, radius + 3.0f, BLACK);
			DrawCircleV(center, radius, WHITE);

			const Vector2 size = MeasureTextEx(font, "?", radius * 1.5f, 1.0f);
			DrawTextEx(font, "?", Vector2{ center.x - size.x / 2.0f, center.y - size.y / 2.0f },
				radius * 1.5f, 1.0f, BLACK);
			break;
		}
		}
	}
}

UpgradeShop::UpgradeShop()
{
	// Levels are saved together in one number: 2 bits per upgrade.
	const int packed = LoadPacked();
	for (int i = 0; i < UPGRADE_COUNT; ++i)
		m_levels[i] = std::clamp((packed >> (2 * i)) & 3, 0, INFO[i].maxLevel);

	// Layout: a row of cards (same as the car shop) and a Back button below.
	const float cardWidth = 200.0f;
	const float cardHeight = 300.0f;
	const float gap = 24.0f;
	const float totalWidth = cardWidth * UPGRADE_COUNT + gap * (UPGRADE_COUNT - 1);
	const float startX = (SCREEN_W - totalWidth) / 2.0f;

	for (int i = 0; i < UPGRADE_COUNT; ++i)
		m_cards[i] = Rectangle{ startX + (cardWidth + gap) * static_cast<float>(i), 190.0f, cardWidth, cardHeight };

	m_backButton = Rectangle{ (SCREEN_W - 200.0f) / 2.0f, 600.0f, 200.0f, 50.0f };
}

UpgradeShop::Event UpgradeShop::Update(CoinManager& coins)
{
	if (m_messageTimer > 0.0f)
		m_messageTimer -= GetFrameTime();

	const Vector2 mouse = GetMousePosition();

	m_hovered = -1;
	for (int i = 0; i < UPGRADE_COUNT; ++i)
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

	const int id = m_hovered;
	const UpgradeInfo& info = INFO[id];

	if (m_levels[id] >= info.maxLevel)
		return Event::None; // already at the maximum

	if (!coins.TrySpend(info.prices[m_levels[id]]))
	{
		m_messageTimer = 1.5f;
		return Event::NotEnoughCoins;
	}

	++m_levels[id];
	Save();
	return Event::Purchased;
}

void UpgradeShop::Draw(const Font& font, int totalCoins) const
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

	for (int i = 0; i < UPGRADE_COUNT; ++i)
	{
		const Rectangle& card = m_cards[i];
		const UpgradeInfo& info = INFO[i];
		const int level = m_levels[i];
		const bool maxed = level >= info.maxLevel;
		const int price = maxed ? 0 : info.prices[level];
		const bool affordable = totalCoins >= price;
		const float cx = card.x + card.width / 2.0f;

		DrawRectangleRec(card, i == m_hovered ? Color{ 60, 60, 75, 255 } : Color{ 40, 40, 50, 255 });
		DrawRectangleLinesEx(card, maxed ? 4.0f : 2.0f, maxed ? GOLD : Color{ 90, 90, 110, 255 });

		DrawUpgradeIcon(font, i, Vector2{ cx, card.y + 52.0f }, 24.0f);
		DrawCentered(font, info.name, cx, card.y + 92.0f, 22, 3, WHITE);
		DrawCentered(font, info.description, cx, card.y + 124.0f, 16, 2, LIGHTGRAY);

		// Level dots
		const float dotGap = 24.0f;
		const float dotsWidth = dotGap * static_cast<float>(info.maxLevel - 1);
		for (int d = 0; d < info.maxLevel; ++d)
		{
			const Vector2 dot{ cx - dotsWidth / 2.0f + dotGap * static_cast<float>(d), card.y + 164.0f };
			DrawCircleV(dot, 9.0f, BLACK);
			DrawCircleV(dot, 7.0f, d < level ? GOLD : Color{ 50, 50, 65, 255 });
		}

		// Current value, and the next one
		const std::string effect = maxed ? ValueText(i, level)
			: ValueText(i, level) + " -> " + ValueText(i, level + 1);
		DrawCentered(font, effect.c_str(), cx, card.y + 186.0f, 20, 2, WHITE);

		if (maxed)
			DrawCentered(font, "MAX", cx, card.y + 220.0f, 22, 3, GOLD);
		else
			DrawPrice(font, price, cx, card.y + 216.0f, affordable ? GOLD : RED);

		const char* hint = maxed ? "" : (affordable ? "Click to upgrade" : "Need more coins");
		DrawCentered(font, hint, cx, card.y + 264.0f, 16, 2, LIGHTGRAY);
	}

	if (m_messageTimer > 0.0f)
		DrawCentered(font, "Not enough coins", centerX, 520.0f, 28, 3, RED);

	// Back button (same look as the other buttons)
	DrawRectangleRounded(m_backButton, 0.7f, 8, m_hoverBack ? ORANGE : YELLOW);
	const Vector2 backSize = MeasureTextEx(font, "Back", 22, 3);
	DrawTextEx(font, "Back",
		Vector2{ m_backButton.x + (m_backButton.width - backSize.x) / 2.0f,
				 m_backButton.y + (m_backButton.height - backSize.y) / 2.0f },
		22, 3, m_hoverBack ? BLACK : RED);
}

float UpgradeShop::GetMagnetDuration() const
{
	return MagnetTimeAt(m_levels[MAGNET_TIME]);
}

float UpgradeShop::GetMagnetRadius() const
{
	return MagnetRangeAt(m_levels[MAGNET_RANGE]);
}

float UpgradeShop::GetSpawnTimeScale() const
{
	return SpawnScaleAt(m_levels[LUCKY_DROPS]);
}

int UpgradeShop::GetShieldCharges() const
{
	return ShieldHitsAt(m_levels[SHIELD_PLUS]);
}

float UpgradeShop::GetSlowDuration() const
{
	return SlowTimeAt(m_levels[SLOW_TIME]);
}

int UpgradeShop::LoadPacked()
{
#if defined(__EMSCRIPTEN__)
	return JS_LoadUpgrades();
#else
	int packed = 0;
	std::ifstream file(UPGRADES_FILE);
	if (file.is_open())
		file >> packed;
	return packed;
#endif
}

void UpgradeShop::Save() const
{
	int packed = 0;
	for (int i = 0; i < UPGRADE_COUNT; ++i)
		packed |= (m_levels[i] & 3) << (2 * i);

#if defined(__EMSCRIPTEN__)
	JS_SaveUpgrades(packed);
#else
	std::ofstream file(UPGRADES_FILE);
	if (file.is_open())
		file << packed;
#endif
}