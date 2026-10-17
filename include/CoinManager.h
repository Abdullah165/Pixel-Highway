#ifndef PIXEL_HIGHWAY_COINMANAGER_H
#define PIXEL_HIGHWAY_COINMANAGER_H

#include <vector>

#include "raylib.h"

// Coins on the road, the coin counters and the saved coin total.
class CoinManager
{
public:
	CoinManager();

	void ResetRun(); // new run: remove all coins and reset this run's counter

	// worldMove = how far the world moved this frame (same value the traffic uses).
	void Update(float worldMove, float frameTime, float roadX, float roadWidth, float worldSpeed);

	// A coin hidden under a traffic car would be impossible to collect safely: remove it.
	void RemoveOverlapping(const Rectangle& rect);

	// Collects every coin touching the car. Returns the position of each collected coin.
	std::vector<Vector2> CollectTouching(const Rectangle& carRect);

	void Draw() const; // the coins on the road (draw inside the camera)
	void DrawHud(const Font& font, float x, float y) const; // icon + this run's coins

	float GetPickupPitch() const; // rises while the player keeps chaining coins
	int GetRunCoins() const;
	int GetTotalCoins() const;

	// Double coins (rewarded ad)
	int GetDoubleableCoins() const; // coins of this run that have not been doubled yet
	void DoubleRunCoins();          // adds a bonus equal to GetDoubleableCoins() and saves

	bool TrySpend(int amount); // spends from the total (shop). Returns false if there is not enough.
	static void DrawIcon(int x, int y, float radius); // small coin icon for menus

	// Magnet: pulls the coins within `radius` toward `target`.
	void PullToward(Vector2 target, float radius, float dt);

	void Save(); // saves the total, only if it changed

private:
	// ---- Tuning ----
	static constexpr float RADIUS = 12.0f;               // drawn size of a coin
	static constexpr float PICKUP_RADIUS = 20.0f;        // collected a bit before the car really touches it
	static constexpr int PATTERN_COUNT = 5;              // coins per pattern
	static constexpr float EDGE_MARGIN = 46.0f;          // keeps coins inside the road
	static constexpr float WAVE_AMPLITUDE = 45.0f;
	static constexpr float FIRST_SPAWN_DISTANCE = 500.0f;
	static constexpr float SPAWN_GAP_MIN = 2000.0f;      // world distance between patterns
	static constexpr float SPAWN_GAP_MAX = 3500.0f;
	static constexpr float CHAIN_TIME = 1.2f;            // seconds to keep a coin chain alive
	static constexpr float PULSE_TIME = 0.2f;            // counter "pop" duration

	// Spacing is measured in TIME, not pixels: the faster the world moves, the further apart
	// the coins are, so the player has the same time to steer on every difficulty.
	static constexpr float FRAMES_BETWEEN_COINS = 12.0f; // about 0.2 seconds at 60 FPS
	static constexpr float MIN_COIN_SPACING = 40.0f;
	static constexpr float PLAYER_STEER_SPEED = 3.0f;    // must match CarController::m_speed
	static constexpr float STEER_EFFORT = 0.5f;          // patterns ask for at most 50% of the steering speed
	static constexpr float MAGNET_PULL_SPEED = 14.0f; // pixels per 60 FPS frame

	struct Coin
	{
		Vector2 position;
		float phase; // makes the coins spin out of sync
	};

	void SpawnPattern(float roadX, float roadWidth, float worldSpeed);
	void DrawCoinShape(Vector2 center, float phase) const;

	int LoadTotal() const;
	void WriteTotal(int total) const;

	std::vector<Coin> m_coins;
	float m_distanceUntilSpawn = FIRST_SPAWN_DISTANCE;
	float m_time = 0.0f;

	int m_runCoins = 0;
	int m_doubledBase = 0; // how many of this run's coins are already counted by a bonus
	int m_totalCoins = 0;
	bool m_totalChanged = false;

	int m_chain = 0;
	float m_chainTimer = 0.0f;
	float m_hudPulse = 0.0f;
};

#endif //PIXEL_HIGHWAY_COINMANAGER_H