#include "../include/CarController.h"

#include <algorithm>
#include <string>
#include "../include/Game.h"


void CarController::InitResources()
{
	if (m_loaded) return;

	m_texture[0] = LoadTexture("assets/textures/car/car_24px_8way_white_6.png"); // left
	m_texture[1] = LoadTexture("assets/textures/car/car_24px_8way_white_7.png"); // straight
	m_texture[2] = LoadTexture("assets/textures/car/car_24px_8way_white_8.png"); // right

	m_position = Vector2{ GetScreenWidth() / 2.2f, GetScreenHeight() - 150.0f };

	for (size_t i = 0; i < m_explosionTextures.size(); i++)
	{
		const std::string filePath = "assets/textures/explosion/frames/1_" + std::to_string(i) + ".png";
		m_explosionTextures[i] = LoadTexture(filePath.c_str());
	}

	m_loaded = true;
}

void CarController::UnloadResources()
{
	if (!m_loaded) return;

	for (const auto& texture : m_texture)
	{
		UnloadTexture(texture);
	}

	for (const auto& texture : m_explosionTextures)
	{
		UnloadTexture(texture);
	}

	m_loaded = false;
}

void CarController::Draw(bool isGameOver, bool isCrashing)
{
	if (m_wasLeftKeyDown && (isGameOver || isCrashing)) 
	{
		DrawTexture(m_texture[0], static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
		if (!m_isExploding) return;
	}
	else if(m_wasRightKeyDown && (isGameOver || isCrashing))
	{
		DrawTexture(m_texture[2], static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
		if(!m_isExploding) return;
	}
	else if(isGameOver || isCrashing)
	{
		DrawTexture(m_texture[1], static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
		if (!m_isExploding) return;
	}

	if (m_isExploding)
	{
		m_explosionTimer += GetFrameTime();

		if (m_explosionTimer >= 0.1f)
		{
			m_explosionFrame++;
			m_explosionTimer = 0.0f;
		}

		if (m_explosionFrame < static_cast<int>(m_explosionTextures.size()))
		{
			DrawTexture(m_explosionTextures[m_explosionFrame],
				static_cast<int>(m_position.x) - 30, static_cast<int>(m_position.y) - 70, WHITE);
		}
		else
		{
			m_isExploding = false;
		}
	}
	else
	{
		if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		{
			DrawTexture(m_texture[2], static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
			m_wasRightKeyDown = true;
		}
		else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
		{
			DrawTexture(m_texture[0], static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
			m_wasLeftKeyDown = true;
		}
		else
		{
			DrawTexture(m_texture[1], static_cast<int>(m_position.x), static_cast<int>(m_position.y), WHITE);
			m_wasLeftKeyDown = false;
			m_wasRightKeyDown = false;
		}
	}
}

void CarController::Update(const Vector2& roadPosition, int roadWidth, float dt)
{
	const float step = m_speed * dt;

	const float minX = roadPosition.x + 20.0f;
	const float maxX = roadPosition.x + roadWidth - 70.0f;
	const float minY = 200.0f;
	const float maxY = static_cast<float>(GetScreenHeight()) - 150.0f;

	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		m_position.x = std::min(m_position.x + step, maxX);
	else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
		m_position.x = std::max(m_position.x - step, minX);
	else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		m_position.y = std::max(m_position.y - step, minY);
	else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		m_position.y = std::min(m_position.y + step, maxY);
}

Rectangle CarController::getRect() const
{
	return Rectangle{ m_position.x + 12, m_position.y + 5,
					 static_cast<float>(m_texture[0].width - 20),
					 static_cast<float>(m_texture[0].height - 15) };
}

void CarController::PlayCarExplosionVFX()
{
	m_isExploding = true;
	m_explosionFrame = 0;
	m_explosionTimer = 0.0f;
}

void CarController::StopExplosionVFX()
{
	m_isExploding = false;
}