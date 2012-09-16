#pragma once

#include "actor.h"

namespace Pacman {

class SpriteSheet;

class GhostActor : public Actor
{
public:

    GhostActor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
               const uint16_t cellSize, const std::weak_ptr<SpriteSheet> spriteSheet,
               const std::string& leftSpriteName, const std::string& rightSpriteName,
               const std::string& topSpriteName, const std::string& bottomSpriteName);
    GhostActor(const GhostActor&) = delete;
    ~GhostActor() = default;

    GhostActor& operator= (const GhostActor&) = delete;

private:

};

} // Pacman namespace