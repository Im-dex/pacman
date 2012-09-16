#include "ghost.h"

namespace Pacman {

GhostActor::GhostActor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
                       const uint16_t cellSize, const std::weak_ptr<SpriteSheet> spriteSheet,
                       const std::string& leftSpriteName, const std::string& rightSpriteName,
                       const std::string& topSpriteName, const std::string& bottomSpriteName)
     : Actor(size, speed, startPosition, cellSize, nullptr)
{
}

} // Pacman namespace