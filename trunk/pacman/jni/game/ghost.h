#pragma once

#include "actor.h"

namespace Pacman {

class Sprite;
class SpriteSheet;

class GhostActor : public Actor
{
public:

    GhostActor(const uint16_t size, const uint16_t speed, const uint16_t cellSize,
               const SpritePosition& startPosition, const MoveDirection startDirection,
               const std::weak_ptr<SpriteSheet> spriteSheet,
               const std::string& leftSpriteName, const std::string& rightSpriteName,
               const std::string& topSpriteName, const std::string& bottomSpriteName);

    GhostActor(const GhostActor&) = delete;
    ~GhostActor() = default;

    GhostActor& operator= (const GhostActor&) = delete;

protected:

    virtual void DirectionChanged(const MoveDirection newDirection);

private:

    const std::shared_ptr<Sprite> mLeftSprite;
    const std::shared_ptr<Sprite> mRightSprite;
    const std::shared_ptr<Sprite> mTopSprite;
    const std::shared_ptr<Sprite> mBottomSprite;
};

} // Pacman namespace