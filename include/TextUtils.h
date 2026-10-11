#ifndef PIXEL_HIGHWAY_TEXTUTILS_H
#define PIXEL_HIGHWAY_TEXTUTILS_H

#include "raylib.h"

// Draws text with a solid outline: the text is drawn 8 times around its position
// in the outline color, then once on top in the real color.
inline void DrawTextOutlined(const Font& font, const char* text, Vector2 position,
    float fontSize, float spacing, Color color, Color outline,
    float thickness = 1.5f)
{
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            if (dx == 0 && dy == 0) continue;

            DrawTextEx(font, text,
                Vector2{ position.x + dx * thickness, position.y + dy * thickness },
                fontSize, spacing, outline);
        }
    }

    DrawTextEx(font, text, position, fontSize, spacing, color);
}

#endif //PIXEL_HIGHWAY_TEXTUTILS_H