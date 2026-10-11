#ifndef PIXEL_HIGHWAY_POPUPMANAGER_H
#define PIXEL_HIGHWAY_POPUPMANAGER_H

#include <string>
#include <vector>

#include "raylib.h"

// Floating text that rises and fades out (for "CLOSE CALL +50" and similar).
class PopupManager
{
public:
    void Add(const std::string& text, Vector2 center, Color color);
    void Update(float deltaSeconds);
    void Draw(const Font& font) const;
    void Clear();

private:
    struct Popup
    {
        std::string text;
        Vector2 position;
        Color color;
        float age;
    };

    static constexpr float LIFETIME = 1.0f;    // seconds on screen
    static constexpr float RISE_SPEED = 60.0f; // pixels per second

    std::vector<Popup> m_popups;
};

#endif //PIXEL_HIGHWAY_POPUPMANAGER_H