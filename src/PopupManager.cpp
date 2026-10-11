#include "../include/PopupManager.h"
#include "../include/TextUtils.h"

#include <algorithm>

void PopupManager::Add(const std::string& text, Vector2 center, Color color)
{
    m_popups.push_back(Popup{ text, center, color, 0.0f });
}

void PopupManager::Update(float deltaSeconds)
{
    for (auto& popup : m_popups)
    {
        popup.age += deltaSeconds;
        popup.position.y -= RISE_SPEED * deltaSeconds;
    }

    m_popups.erase(
        std::remove_if(m_popups.begin(), m_popups.end(),
            [](const Popup& popup) { return popup.age >= LIFETIME; }),
        m_popups.end());
}

void PopupManager::Draw(const Font& font) const
{
    for (const auto& popup : m_popups)
    {
        const float alpha = 1.0f - popup.age / LIFETIME;

        // The outline fades much faster than the text (alpha to the 4th power),
        // so it is gone before it can darken the fading letters.
        const float outlineAlpha = alpha * alpha * alpha * alpha;

        const Vector2 size = MeasureTextEx(font, popup.text.c_str(), 26, 3);

        DrawTextOutlined(font, popup.text.c_str(),
            Vector2{ popup.position.x - size.x / 2.0f, popup.position.y },
            26, 3, ColorAlpha(popup.color, alpha), ColorAlpha(BLACK, outlineAlpha));
    }
}

void PopupManager::Clear()
{
    m_popups.clear();
}