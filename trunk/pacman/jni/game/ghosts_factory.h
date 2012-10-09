#pragma once

#include <cstdint>
#include <memory>

#include "base.h"

namespace Pacman {

class Ghost;
class GameLoader;
class Map;
class SpriteSheet;

class GhostsFactory
{
public:

    static const size_t kBlinky;
    static const size_t kPinky;
    static const size_t kInky;
    static const size_t kClyde;

    GhostsFactory() = default;
    GhostsFactory(const GhostsFactory&) = delete;
    ~GhostsFactory() = default;

    GhostsFactory& operator= (const GhostsFactory&) = delete;

    std::shared_ptr<Ghost> CreateGhost(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr,
                                       const size_t ghostId) const;
};

} // Pacman namespace