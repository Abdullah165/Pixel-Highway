#ifndef PIXEL_HIGHWAY_NPCCARCONTROLLER_H
#define PIXEL_HIGHWAY_NPCCARCONTROLLER_H

#include "raylib.h"


class NpcCarController
{
public:
    NpcCarController();
    NpcCarController(Texture2D* carTexture, Vector2 position);

    void Draw() const;
    void Update(const Vector2& roadPosition, int roadWidth, float worldSpeed);
    void Respawn(const Vector2& roadPosition, int roadWidth);
    Rectangle getRect() const;
    void SetPosition(const Vector2& position);

    // Near-miss tracking (used by Game::CheckNearMisses)
    bool WasNear() const;
    void SetWasNear(bool value);
    bool IsNearMissHandled() const;
    void MarkNearMissHandled();

private:
    void ResetNearMiss();

    Texture2D* m_texture;
    Vector2 m_position;

    bool m_wasNear = false;          // touched the player's danger zone
    bool m_nearMissHandled = false;  // already counted (or disqualified) for this pass
};


#endif //PIXEL_HIGHWAY_NPCCARCONTROLLER_H