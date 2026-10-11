#include "../include/ScoreManager.h"
#include "../include/TextUtils.h"
#include <algorithm>
#include <fstream>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
EM_JS(int, JS_LoadBest, (), {
	try { return parseInt(localStorage.getItem("ph_best") || "0"); }
 catch (e) { return 0; }
	});
EM_JS(void, JS_SaveBest, (int v), {
	try { localStorage.setItem("ph_best", v.toString()); }
 catch (e) {}
	});
#endif

namespace
{
	constexpr const char* BEST_SCORE_FILE = "best_score.txt"; // desktop only
}

ScoreManager::ScoreManager()
{
	m_bestScore = LoadBestScore();
}

void ScoreManager::ResetCurrentScore()
{
	m_currentScore = 0.0f;
	ResetCombo();
}

void ScoreManager::Update()
{
	const float frameTime = GetFrameTime();

	// The combo runs out if the player does not take another risk in time.
	if (m_combo > 0)
	{
		m_comboTimer -= frameTime;
		if (m_comboTimer <= 0.0f)
			ResetCombo();
	}

	m_currentScore += frameTime * 10.0f * GetMultiplier();

	UpdateBest();
}

int ScoreManager::RegisterNearMiss()
{
	const int bonus = NEAR_MISS_BONUS * GetMultiplier(); // paid at the multiplier you had

	m_currentScore += static_cast<float>(bonus);
	m_combo = std::min(m_combo + 1, MAX_MULTIPLIER - 1);
	m_comboTimer = COMBO_TIME;

	UpdateBest();
	return bonus;
}

void ScoreManager::ResetCombo()
{
	m_combo = 0;
	m_comboTimer = 0.0f;
}

int ScoreManager::GetMultiplier() const
{
	return std::min(1 + m_combo, MAX_MULTIPLIER);
}

void ScoreManager::Draw(const Font& font) const
{
	DrawTextOutlined(font, TextFormat("Score: %i", static_cast<int>(m_currentScore)),
		Vector2{ 300.0f, 20.0f }, 30, 5, WHITE, BLACK);

	DrawTextOutlined(font, TextFormat("Best Score: %i", m_bestScore),
		Vector2{ GetScreenWidth() / 1.6f, 20.0f }, 30, 5, WHITE, BLACK);

	// Combo text and a bar showing how much time is left
	if (m_combo > 0)
	{
		DrawTextOutlined(font, TextFormat("Combo x%i", GetMultiplier()),
			Vector2{ 300.0f, 60.0f }, 26, 4, ORANGE, BLACK);

		const int barWidth = 160;
		const float fraction = std::clamp(m_comboTimer / COMBO_TIME, 0.0f, 1.0f);
		DrawRectangle(300, 95, barWidth, 8, ColorAlpha(BLACK, 0.5f));
		DrawRectangle(300, 95, static_cast<int>(barWidth * fraction), 8, ORANGE);
	}
}

void ScoreManager::UpdateBest()
{
	if (static_cast<int>(m_currentScore) > m_bestScore)
	{
		m_bestScore = static_cast<int>(m_currentScore);
		m_bestScoreChanged = true;
	}
}

void ScoreManager::SaveBestScore()
{
	if (!m_bestScoreChanged) return;

	WriteBestScore(m_bestScore);
	m_bestScoreChanged = false;
}

int ScoreManager::LoadBestScore() const
{
#if defined(__EMSCRIPTEN__)
	return JS_LoadBest();
#else
	std::ifstream file(BEST_SCORE_FILE);
	int bestScore = 0;
	if (file.is_open())
		file >> bestScore;
	return bestScore;
#endif
}

void ScoreManager::WriteBestScore(int bestScore) const
{
#if defined(__EMSCRIPTEN__)
	JS_SaveBest(bestScore);
#else
	std::ofstream file(BEST_SCORE_FILE);
	if (file.is_open())
		file << bestScore;
#endif
}