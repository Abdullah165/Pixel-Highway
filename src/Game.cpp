#include "../include/Game.h"
#include "../include/TextUtils.h"

#include <ctime>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

namespace
{
	// Must stay in the same order as Game::TextureID.
	constexpr std::array<const char*, Game::TextureCount> TEXTURE_FILES = {
		"assets/textures/car/blue_car.png",
		"assets/textures/car/green_car.png",
		"assets/textures/car/yellow_car.png",
		"assets/textures/car/pink_car.png",
		"assets/textures/car/grey_car.png",
		"assets/textures/car/wreck_car.png",
		"assets/textures/scenery/tree.png",
		"assets/textures/scenery/palm_tree.png"
	};

	float GetStartSpeed(Difficulty difficulty)
	{
		switch (difficulty)
		{
		case Difficulty::Easy:   return 3.0f;
		case Difficulty::Medium: return 5.0f;
		case Difficulty::Hard:   return 7.0f;
		}
		return 5.0f;
	}
}

Game::Game()
{
	Init();
}

Game::~Game()
{
	Cleanup();
}

void Game::Init()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Highway");
	SetTargetFPS(60);
	SetRandomSeed(static_cast<unsigned int>(std::time(nullptr)));

	m_font = LoadFont("assets/fonts/JungleAdventurer.otf");

	for (int i = 0; i < TextureCount; ++i)
	{
		m_textures[i] = LoadTexture(TEXTURE_FILES[i]);
	}

	m_car_controller.InitResources();
	m_road.InitResources();

	CreateNpcCars();
	CreateScenery();

	m_gameOver.Init(m_road.GetPosition().x, static_cast<float>(m_road.GetWidth()));


	// Difficulty buttons, centered in a row
	const float buttonWidth = 150.0f;
	const float buttonHeight = 50.0f;
	const float gap = 20.0f;
	const float startX = (SCREEN_WIDTH - (buttonWidth * 3.0f + gap * 2.0f)) / 2.0f;
	const float buttonY = SCREEN_HEIGHT / 1.7f;

	m_easyButtonBounds = Rectangle{ startX, buttonY, buttonWidth, buttonHeight };
	m_mediumButtonBounds = Rectangle{ startX + (buttonWidth + gap), buttonY, buttonWidth, buttonHeight };
	m_hardButtonBounds = Rectangle{ startX + (buttonWidth + gap) * 2.0f, buttonY, buttonWidth, buttonHeight };

	m_garageButtonBounds = Rectangle{ (SCREEN_WIDTH - 200.0f) / 2.0f, 570.0f, 200.0f, 50.0f };

	// Garage tabs (top left, away from the title)
	m_tabCarsBounds = Rectangle{ 60.0f, 40.0f, 150.0f, 44.0f };
	m_tabUpgradesBounds = Rectangle{ 224.0f, 40.0f, 170.0f, 44.0f };

	// Use the color equipped in the garage
	m_car_controller.SetTint(m_shop.GetSelectedTint());
}

void Game::Cleanup()
{
	m_car_controller.UnloadResources();
	m_road.UnloadResources();

	for (auto& texture : m_textures)
	{
		UnloadTexture(texture);
	}

	m_sound.ShutdownAllSounds();

	UnloadFont(m_font);

	CloseWindow();
}

void Game::Run()
{
	// Everything is loaded: tell CrazyGames the loading screen can go away.
	CrazySDK::LoadingFinished();

#if defined(__EMSCRIPTEN__)
	emscripten_set_main_loop_arg(
		[](void* game) { static_cast<Game*>(game)->UpdateDrawFrame(); }, this, 0, 1);
#else
	while (!WindowShouldClose())
		UpdateDrawFrame();
#endif
}

void Game::UpdateDrawFrame()
{
	// The browser blocks audio until the player interacts with the page,
	// so we wait for the first click / key press before starting.
	if (!m_started)
	{
		if (m_inGarage)
			DrawGarageScreen();
		else
			DrawStartScreen();
		return;
	}

	// While an ad is playing the game is frozen (the ad is shown on top of the page).
	if (CrazySDK::IsAdPlaying())
	{
		DrawGame();
		return;
	}

	// dt = 1.0 means "one frame at 60 FPS". This keeps the game speed the same
	// on 60 Hz, 144 Hz and slower computers.
	const float dt = std::min(GetFrameTime() * 60.0f, MAX_FRAME_SCALE);

	UpdateGame(dt);
	DrawGame();
}

void Game::DrawStartScreen()
{
	const Vector2 mouse = GetMousePosition();

	const bool hoverEasy = CheckCollisionPointRec(mouse, m_easyButtonBounds);
	const bool hoverMedium = CheckCollisionPointRec(mouse, m_mediumButtonBounds);
	const bool hoverHard = CheckCollisionPointRec(mouse, m_hardButtonBounds);
	const bool hoverGarage = CheckCollisionPointRec(mouse, m_garageButtonBounds);

	// Only a click on one of the buttons does something.
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (hoverEasy)
			StartGame(Difficulty::Easy);
		else if (hoverMedium)
			StartGame(Difficulty::Medium);
		else if (hoverHard)
			StartGame(Difficulty::Hard);
		else if (hoverGarage)
		{
			m_sound.InitResources(); // the click unlocks audio, so the garage can play sounds
			m_sound.StartPlayingSound("ButtonClick");
			m_inGarage = true;
		}
	}

	BeginDrawing();
	ClearBackground(BLACK);

	const char* text = "Select Difficulty to Start";
	const Vector2 size = MeasureTextEx(m_font, text, 40, 4);
	DrawTextEx(m_font, text,
		Vector2{ (SCREEN_WIDTH - size.x) / 2.0f, (SCREEN_HEIGHT - size.y) / 2.0f },
		40, 4, WHITE);

	// Normal: yellow button with red text. Hover: orange button with black text.
	DrawButton(m_font, m_easyButtonBounds, "Easy",
		hoverEasy ? ORANGE : YELLOW, hoverEasy ? BLACK : RED);
	DrawButton(m_font, m_mediumButtonBounds, "Medium",
		hoverMedium ? ORANGE : YELLOW, hoverMedium ? BLACK : RED);
	DrawButton(m_font, m_hardButtonBounds, "Hard",
		hoverHard ? ORANGE : YELLOW, hoverHard ? BLACK : RED);
	DrawButton(m_font, m_garageButtonBounds, "Garage",
		hoverGarage ? ORANGE : YELLOW, hoverGarage ? BLACK : RED);

	// Best score of each difficulty, centered under its button
	const auto drawBest = [this](const Rectangle& bounds, Difficulty difficulty)
	{
		const char* label = TextFormat("Best: %i", m_score.GetBestScore(difficulty));
		const Vector2 labelSize = MeasureTextEx(m_font, label, 20, 3);
		DrawTextEx(m_font, label,
			Vector2{ bounds.x + (bounds.width - labelSize.x) / 2.0f, bounds.y + bounds.height + 10.0f },
			20, 3, LIGHTGRAY);
	};

	drawBest(m_easyButtonBounds, Difficulty::Easy);
	drawBest(m_mediumButtonBounds, Difficulty::Medium);
	drawBest(m_hardButtonBounds, Difficulty::Hard);

	// Total coins, top right
	const char* coinsText = TextFormat("%i", m_coins.GetTotalCoins());
	const Vector2 coinsSize = MeasureTextEx(m_font, coinsText, 28, 4);
	const float coinsRight = SCREEN_WIDTH - 40.0f;
	DrawTextOutlined(m_font, coinsText, Vector2{ coinsRight - coinsSize.x, 30.0f }, 28, 4, GOLD, BLACK);
	CoinManager::DrawIcon(static_cast<int>(coinsRight - coinsSize.x - 26.0f), 46, 13.0f);

	EndDrawing();
}

void Game::DrawGarageScreen()
{
	const Vector2 mouse = GetMousePosition();
	const bool hoverCarsTab = CheckCollisionPointRec(mouse, m_tabCarsBounds);
	const bool hoverUpgradesTab = CheckCollisionPointRec(mouse, m_tabUpgradesBounds);

	// Tabs
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (hoverCarsTab && m_garageTab != GarageTab::Cars)
		{
			m_garageTab = GarageTab::Cars;
			m_sound.StartPlayingSound("ButtonClick");
		}
		else if (hoverUpgradesTab && m_garageTab != GarageTab::Upgrades)
		{
			m_garageTab = GarageTab::Upgrades;
			m_sound.StartPlayingSound("ButtonClick");
		}
	}

	if (m_garageTab == GarageTab::Cars)
	{
		switch (m_shop.Update(m_coins))
		{
		case CarShop::Event::Back:
			m_sound.StartPlayingSound("ButtonClick");
			m_inGarage = false;
			break;
		case CarShop::Event::Equipped:
			m_sound.StartPlayingSound("ButtonClick");
			m_car_controller.SetTint(m_shop.GetSelectedTint());
			break;
		case CarShop::Event::Purchased:
			m_sound.PlaySoundWithPitch("Coin", 1.9f);
			m_car_controller.SetTint(m_shop.GetSelectedTint());
			break;
		case CarShop::Event::NotEnoughCoins:
			m_sound.PlaySoundWithPitch("CloseCall", 0.6f); // a low "nope" sound
			break;
		case CarShop::Event::None:
			break;
		}
	}
	else
	{
		switch (m_upgrades.Update(m_coins))
		{
		case UpgradeShop::Event::Back:
			m_sound.StartPlayingSound("ButtonClick");
			m_inGarage = false;
			break;
		case UpgradeShop::Event::Purchased:
			m_sound.PlaySoundWithPitch("Coin", 1.9f);
			break;
		case UpgradeShop::Event::NotEnoughCoins:
			m_sound.PlaySoundWithPitch("CloseCall", 0.6f);
			break;
		case UpgradeShop::Event::None:
			break;
		}
	}

	BeginDrawing();
	ClearBackground(Color{ 20, 20, 28, 255 });

	if (m_garageTab == GarageTab::Cars)
		m_shop.Draw(m_font, m_car_controller.GetPreviewTexture(), m_coins.GetTotalCoins());
	else
		m_upgrades.Draw(m_font, m_coins.GetTotalCoins());

	// Tab buttons: the open tab is orange, the others are dark (yellow when hovered)
	const auto drawTab = [this](const Rectangle& bounds, const char* label, bool active, bool hover)
	{
		if (active)
			DrawButton(m_font, bounds, label, ORANGE, BLACK);
		else if (hover)
			DrawButton(m_font, bounds, label, YELLOW, RED);
		else
			DrawButton(m_font, bounds, label, Color{ 60, 60, 75, 255 }, LIGHTGRAY);
	};

	drawTab(m_tabCarsBounds, "Cars", m_garageTab == GarageTab::Cars, hoverCarsTab);
	drawTab(m_tabUpgradesBounds, "Upgrades", m_garageTab == GarageTab::Upgrades, hoverUpgradesTab);

	EndDrawing();
}

void Game::StartGame(Difficulty difficulty)
{
	m_difficulty = difficulty;
	m_startSpeed = GetStartSpeed(difficulty);

	m_sound.InitResources(); // audio starts AFTER the user's click
	m_sound.StartPlayingSound("ButtonClick");
	m_started = true;

	m_score.SetDifficulty(difficulty); // picks that difficulty's best score
	ResetGame(); // applies the start speed and calls CrazySDK::GameplayStart()
}

void Game::UpdateGame(float dt)
{
	const float frameTime = GetFrameTime();

	if (m_noAdMessageTimer > 0.0f)
		m_noAdMessageTimer -= frameTime;

	m_popups.Update(frameTime);
	m_shake.Update(frameTime);

	// Crash sequence: everything stays frozen while the explosion and the shake play,
	// then the Game Over screen appears.
	if (m_isCrashing)
	{
		m_crashTimer -= frameTime;

		if (m_crashTimer <= 0.0f)
		{
			m_isCrashing = false;
			m_isGameOver = true;
		}
		return;
	}

	if (!m_isGameOver)
	{
		// Sounds
		if (!m_sound.IsSoundStillPlaying("Racing"))
			m_sound.StartPlayingSound("Racing");

		if (!m_sound.IsSoundStillPlaying("Traffic"))
			m_sound.StartPlayingSound("Traffic");

		m_car_controller.Update(m_road.GetPosition(), m_road.GetWidth(), dt, m_worldSpeed);

		// Slow motion slows everything that moves with the world. The car's steering does not change.
		const float timeScale = m_powerUps.GetTimeScale();
		const float worldMove = m_worldSpeed * timeScale * dt;

		if (m_invincibleTimer > 0.0f)
			m_invincibleTimer -= frameTime;

		// Gradually increase the world speed every WORLD_SPEED_INTERVAL seconds,
		// until reaching MAX_WORLD_SPEED.
		m_worldSpeedTimer += frameTime;
		if (m_worldSpeedTimer > WORLD_SPEED_INTERVAL)
		{
			m_worldSpeed = std::min(m_worldSpeed + 1.0f, MAX_WORLD_SPEED);
			m_worldSpeedTimer = 0.0f;
		}

		for (auto& npc_car : m_npc_cars)
		{
			npc_car.Update(m_road.GetPosition(), m_road.GetWidth(), worldMove);

			if (m_invincibleTimer <= 0.0f &&
				CheckCollisionRecs(m_car_controller.getRect(), npc_car.getRect()))
			{
				if (m_powerUps.ConsumeShield())
				{
					OnShieldBroken(); // the shield took the hit: no crash
				}
				else
				{
					OnCrash();
					break;
				}
			}
		}

		if (m_isCrashing)
			return; // the crash sequence starts now

		CheckNearMisses();
		UpdateCollectibles(dt, frameTime);

		for (auto& scenery : m_sceneries)
		{
			scenery.Update(worldMove);
		}

		m_score.Update(timeScale); // the passive score grows slower during slow motion
	}
	else
	{
		// Game Over buttons (only active on the Game Over screen)
		const Vector2 mouse = GetMousePosition();

		const bool hoverPlayAgain = CheckCollisionPointRec(mouse, m_gameOver.GetPlayAgainButtonRec());
		const bool hoverMenu = CheckCollisionPointRec(mouse, m_gameOver.GetMenuButtonRec());
		const bool hoverContinue = !m_continueUsed &&
			CheckCollisionPointRec(mouse, m_gameOver.GetContinueButtonRec());
		const bool hoverDouble = GetDoubleCoinsOffer() > 0 &&
			CheckCollisionPointRec(mouse, m_gameOver.GetDoubleButtonRec());

		m_gameOver.ChangePlayAgainTextColor(hoverPlayAgain ? WHITE : RED);
		m_gameOver.ChangeMenuTextColor(hoverMenu ? WHITE : RED);
		m_gameOver.ChangeContinueTextColor(hoverContinue ? WHITE : RED);
		m_gameOver.ChangeDoubleTextColor(hoverDouble ? WHITE : GOLD);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (hoverDouble)
				OnDoubleCoinsPressed();
			else if (hoverContinue)
				OnContinuePressed();
			else if (hoverPlayAgain)
				OnPlayAgainPressed();
			else if (hoverMenu)
				OnMenuPressed();
		}
	}
}


void Game::OnContinuePressed()
{
	m_sound.StartPlayingSound("ButtonClick");

	// Rewarded ad: the player chose to watch it, the reward is another chance.
	CrazySDK::RequestAd(CrazySDK::AdType::Rewarded, [this](bool completed)
	{
		if (completed)
			ContinueRun();
		else
			m_noAdMessageTimer = 3.0f; // ad failed / not available
	});
}

int Game::GetDoubleCoinsOffer() const
{
	const int bonus = m_coins.GetDoubleableCoins();
	return bonus >= MIN_COINS_TO_OFFER_DOUBLE ? bonus : 0;
}

void Game::OnDoubleCoinsPressed()
{
	m_sound.StartPlayingSound("ButtonClick");

	// Rewarded ad: the player chose to watch it, the reward is a bonus equal to this run's coins.
	CrazySDK::RequestAd(CrazySDK::AdType::Rewarded, [this](bool completed)
	{
		if (completed)
		{
			m_coins.DoubleRunCoins();
			m_watchedRewardedAd = true;
			m_sound.PlaySoundWithPitch("Coin", 1.9f);
		}
		else
		{
			m_noAdMessageTimer = 3.0f; // ad failed / not available
		}
	});
}

void Game::OnPlayAgainPressed()
{
	m_sound.StartPlayingSound("ButtonClick");

	//A rewarded ad was already watched in this run
	if (m_watchedRewardedAd)
	{
		ResetGame();
		return;
	}

	// Midgame ad at the natural break. The new run starts when the ad ends,
	// even if no ad was available.
	CrazySDK::RequestAd(CrazySDK::AdType::Midgame, [this](bool)
	{
		ResetGame();
	});
}

void Game::OnMenuPressed()
{
	m_sound.StartPlayingSound("ButtonClick");

	//A rewarded ad was already watched in this run
	if (m_watchedRewardedAd)
	{
		ReturnToMenu();
		return;
	}

	CrazySDK::RequestAd(CrazySDK::AdType::Midgame, [this](bool)
	{
		ReturnToMenu(); // also when no ad was available
	});
}

void Game::ReturnToMenu()
{
	m_sound.StopPlayingSound("Racing");
	m_sound.StopPlayingSound("Traffic");

	m_shake.Stop();
	m_noAdMessageTimer = 0.0f;

	// The start screen is drawn whenever m_started is false.
	// Choosing a difficulty there calls StartGame(), which resets the whole run.
	m_started = false;
}

void Game::ContinueRun()
{
	m_continueUsed = true;
	m_isGameOver = false;
	m_watchedRewardedAd = true;
	m_worldSpeed = m_speedBeforeCrash;
	m_invincibleTimer = CONTINUE_PROTECTION_TIME;

	m_car_controller.StopExplosionVFX();
	m_shake.Stop();
	PlaceNpcCars(); // move the traffic above the screen so the road ahead is clear

	CrazySDK::GameplayStart();
}

void Game::CheckNearMisses()
{
	const Rectangle carRect = m_car_controller.getRect();

	// The "danger zone": the car's hitbox plus a margin on every side.
	const Rectangle dangerZone = Rectangle{
		carRect.x - NEAR_MISS_MARGIN_X,
		carRect.y - NEAR_MISS_MARGIN_Y,
		carRect.width + NEAR_MISS_MARGIN_X * 2.0f,
		carRect.height + NEAR_MISS_MARGIN_Y * 2.0f
	};
	const float carBottom = carRect.y + carRect.height;

	for (auto& npc_car : m_npc_cars)
	{
		if (npc_car.IsNearMissHandled())
			continue;

		const Rectangle npcRect = npc_car.getRect();

		// Touching the car (possible during the Continue protection) is never a close call.
		if (CheckCollisionRecs(carRect, npcRect))
		{
			npc_car.MarkNearMissHandled();
			continue;
		}

		// Did this car come close to the player?
		if (CheckCollisionRecs(dangerZone, npcRect))
			npc_car.SetWasNear(true);

		// The car was close and has now passed the player: reward it.
		if (npc_car.WasNear() && npcRect.y > carBottom)
		{
			const int bonus = m_score.RegisterNearMiss();

			// The pitch rises with the combo: x2 sounds normal, x5 sounds clearly higher.
			const float pitch = 1.0f + 0.12f * (m_score.GetMultiplier() - 2);
			m_sound.PlaySoundWithPitch("CloseCall", pitch);

			m_popups.Add(TextFormat("CLOSE CALL +%i", bonus),
				Vector2{ carRect.x + carRect.width / 2.0f, carRect.y - 20.0f },
				YELLOW);

			npc_car.MarkNearMissHandled();
		}
	}
}

void Game::OnCrash()
{
	m_car_controller.PlayCarExplosionVFX();
	m_sound.StartPlayingSound("CarCrash");

	m_speedBeforeCrash = m_worldSpeed; // remembered for "Continue"
	m_worldSpeed = 0.0f;

	m_score.ResetCombo();   // a crash breaks the combo
	m_score.SaveBestScore(); // save once, when the run ends
	m_coins.Save();

	m_isCrashing = true;
	m_crashTimer = GAME_OVER_DELAY;
	m_shake.Start(CRASH_SHAKE_DURATION, CRASH_SHAKE_STRENGTH);

	CrazySDK::GameplayStop();
}

void Game::UpdateCollectibles(float dt, float frameTime)
{
	const float roadX = m_road.GetPosition().x;
	const float roadWidth = static_cast<float>(m_road.GetWidth());
	const Rectangle carRect = m_car_controller.getRect();
	const Vector2 carCenter{ carRect.x + carRect.width / 2.0f, carRect.y + carRect.height / 2.0f };

	// How far the world moves this frame (smaller during slow motion)
	const float worldMove = m_worldSpeed * m_powerUps.GetTimeScale() * dt;

	// Traffic rectangles: used to keep pickups and coins from sitting under a car.
	std::vector<Rectangle> traffic;
	traffic.reserve(m_npc_cars.size());
	for (const auto& npc_car : m_npc_cars)
		traffic.push_back(npc_car.getRect());

	// Power-ups
	m_powerUps.Update(worldMove, frameTime, roadX, roadWidth, traffic);

	for (const PowerUpManager::Type type : m_powerUps.CollectTouching(carRect))
	{
		m_popups.Add(PowerUpManager::GetName(type),
			Vector2{ carCenter.x, carRect.y - 30.0f }, PowerUpManager::GetColor(type));

		// Same sound file as the coins and close calls, at a pitch of its own
		float pitch = 1.7f; // shield
		if (type == PowerUpManager::Type::Magnet) pitch = 2.0f;
		else if (type == PowerUpManager::Type::SlowMotion) pitch = 0.9f; // lower and calmer
		m_sound.PlaySoundWithPitch("CloseCall", pitch);
	}

	SyncSlowMotionAudio();

	// Coins (the last argument is the normal world speed: it decides how far apart the coins are placed)
	m_coins.Update(worldMove, frameTime, roadX, roadWidth, m_worldSpeed);

	if (m_powerUps.IsMagnetActive())
		m_coins.PullToward(carCenter, m_powerUps.GetMagnetRadius(), dt);

	for (const Rectangle& rect : traffic)
		m_coins.RemoveOverlapping(rect);

	const std::vector<Vector2> collected = m_coins.CollectTouching(carRect);
	if (!collected.empty())
	{
		m_sound.PlaySoundWithPitch("Coin", m_coins.GetPickupPitch());

		for (const Vector2& position : collected)
			m_popups.Add("+1", Vector2{ position.x, position.y - 24.0f }, GOLD);
	}
}

void Game::OnShieldBroken()
{
	m_invincibleTimer = SHIELD_BREAK_PROTECTION;
	m_shake.Start(0.25f, 6.0f); // a small shake: much weaker than a real crash

	// A low, dull version of the close-call sound
	m_sound.PlaySoundWithPitch("CloseCall", 0.5f);

	const Rectangle carRect = m_car_controller.getRect();
	m_popups.Add(m_powerUps.HasShield() ? "SHIELD HIT!" : "SHIELD BROKEN",
		Vector2{ carRect.x + carRect.width / 2.0f, carRect.y - 30.0f },
		PowerUpManager::GetColor(PowerUpManager::Type::Shield));
}

void Game::SyncSlowMotionAudio()
{
	const bool slow = m_powerUps.IsSlowMotionActive();
	if (slow == m_slowAudioOn) return; // only change the pitch when the state changes

	m_slowAudioOn = slow;

	const float pitch = slow ? 0.75f : 1.0f; // lower pitch = slower, deeper sound
	m_sound.SetPitch("Racing", pitch);
	m_sound.SetPitch("Traffic", pitch);
}

void Game::DrawGame()
{
	m_camera.offset = m_shake.GetOffset();

	BeginDrawing();
	ClearBackground(m_selectedBackgroundColor);

	// Everything inside Mode2D is shaken by the camera offset.
	BeginMode2D(m_camera);

	m_road.Draw();
	m_coins.Draw();
	m_powerUps.DrawPickups(m_font);

	for (const auto& npc_car : m_npc_cars)
	{
		npc_car.Draw();
	}

	m_car_controller.Draw(m_isGameOver, m_isCrashing);

	if (!m_isGameOver && !m_isCrashing)
		m_powerUps.DrawCarEffects(m_car_controller.getRect(), m_invincibleTimer > 0.0f);

	for (const auto& scenery : m_sceneries)
	{
		scenery.Draw();
	}

	EndMode2D();

	// Slow motion tint (outside the camera, so it does not shake)
	if (!m_isGameOver)
		m_powerUps.DrawScreenEffects();

	// The interface is drawn outside the camera, so it stays steady and readable.
	m_popups.Draw(m_font);
	m_score.Draw(m_font);
	m_coins.DrawHud(m_font, GetScreenWidth() / 1.6f, 88.0f);

	if (!m_isGameOver)
		m_powerUps.DrawHud(m_font, GetScreenWidth() / 1.6f, 130.0f);

	if (m_isGameOver)
	{
		m_gameOver.Draw(m_font, !m_continueUsed, GetDoubleCoinsOffer());

		// Coins earned in this run
		const float centerX = m_road.GetPosition().x + m_road.GetWidth() / 2.0f;
		const char* runText = TextFormat("Coins +%i", m_coins.GetRunCoins());
		const char* totalText = TextFormat("Total %i", m_coins.GetTotalCoins());
		const Vector2 runSize = MeasureTextEx(m_font, runText, 24, 3);
		const Vector2 totalSize = MeasureTextEx(m_font, totalText, 24, 3);

		DrawTextOutlined(m_font, runText,
			Vector2{ centerX - runSize.x / 2.0f, 248.0f }, 24, 3, GOLD, BLACK);
		DrawTextOutlined(m_font, totalText,
			Vector2{ centerX - totalSize.x / 2.0f, 276.0f }, 24, 3, WHITE, BLACK);

		if (m_noAdMessageTimer > 0.0f)
		{
			const char* message = "No ad available";
			const Vector2 size = MeasureTextEx(m_font, message, 22, 3);
			const float roadX = m_road.GetPosition().x;
			DrawTextEx(m_font, message,
				Vector2{ roadX + (m_road.GetWidth() - size.x) / 2.0f, 600.0f }, 22, 3, WHITE); 
		}
	}

	EndDrawing();
}

void Game::ResetGame()
{
	m_isGameOver = false;
	m_isCrashing = false;
	m_crashTimer = 0.0f;
	m_shake.Stop();

	m_worldSpeed = m_startSpeed;
	m_worldSpeedTimer = 0.0f;

	m_continueUsed = false;
	m_watchedRewardedAd = false;
	m_invincibleTimer = 0.0f;
	m_noAdMessageTimer = 0.0f;

	m_car_controller.StopExplosionVFX();
	m_score.ResetCurrentScore(); // also resets the combo
	m_popups.Clear();
	m_coins.ResetRun();

	m_selectedBackgroundColor = GetRandomBackgroundColor();

	PlaceNpcCars();
	PlaceSceneries();

	m_powerUps.SetUpgrades(m_upgrades.GetMagnetDuration(), m_upgrades.GetMagnetRadius(),
		m_upgrades.GetSlowDuration(), m_upgrades.GetSpawnTimeScale(), m_upgrades.GetShieldCharges());
	m_powerUps.ResetRun();
	SyncSlowMotionAudio(); // restores the normal sound pitch if a run ended during slow motion

	CrazySDK::GameplayStart();
}

Color Game::GetRandomBackgroundColor() const
{
	return m_backgroundColors[GetRandomValue(0, static_cast<int>(m_backgroundColors.size()) - 1)];
}

void Game::CreateNpcCars()
{
	// BlueCar..WreckCar are the first 6 textures.
	for (size_t i = 0; i < m_npc_cars.size(); ++i)
	{
		m_npc_cars[i] = NpcCarController(&m_textures[BlueCar + i], Vector2{ 0.0f, 0.0f });
	}

	PlaceNpcCars();
}

void Game::CreateScenery()
{
	m_sceneries[0] = SceneryController(&m_textures[Tree], Vector2{ 0.0f, 0.0f });     // right side
	m_sceneries[1] = SceneryController(&m_textures[PalmTree], Vector2{ 0.0f, 0.0f }); // right side
	m_sceneries[2] = SceneryController(&m_textures[PalmTree], Vector2{ 0.0f, 0.0f }); // left side
	m_sceneries[3] = SceneryController(&m_textures[Tree], Vector2{ 0.0f, 0.0f });     // left side

	PlaceSceneries();
}

void Game::PlaceNpcCars()
{
	// x = screen width / divisor, y = start height above the screen
	static constexpr float X_DIVISOR[6] = { 1.85f, 2.0f, 2.4f, 2.25f, 2.26f, 2.25f };
	static constexpr float START_Y[6] = { -50.0f, -150.0f, -250.0f, -350.0f, -450.0f, -550.0f };

	for (size_t i = 0; i < m_npc_cars.size(); ++i)
	{
		m_npc_cars[i].SetPosition(Vector2{ SCREEN_WIDTH / X_DIVISOR[i], START_Y[i] });
	}
}

void Game::PlaceSceneries()
{
	const float roadX = m_road.GetPosition().x;
	const float roadWidth = static_cast<float>(m_road.GetWidth());

	// Right side of the road
	m_sceneries[0].SetPosition(Vector2{ roadX + roadWidth - 45.0f, -50.0f });
	m_sceneries[1].SetPosition(Vector2{ roadX + roadWidth - 45.0f, -350.0f });

	// Left side of the road
	m_sceneries[2].SetPosition(Vector2{ roadX - 15.0f, -200.0f });
	m_sceneries[3].SetPosition(Vector2{ roadX - 15.0f, -500.0f });
}

void Game::DrawButton(const Font& font, const Rectangle& bounds, const char* text,
	Color fillColor, Color textColor) const
{
	DrawRectangleRounded(bounds, 0.7f, 8, fillColor);

	const float fontSize = 22.0f;
	const float spacing = 3.0f;
	const Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
	DrawTextEx(font, text,
		Vector2{ bounds.x + (bounds.width - size.x) / 2.0f, bounds.y + (bounds.height - size.y) / 2.0f },
		fontSize, spacing, textColor);
}