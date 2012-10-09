#pragma once

#include <cstdint>
#include <memory>

#include "base.h"
#include "game_typedefs.h"

namespace Pacman {

class Ghost;
class GameLoader;
class Map;
class SpriteSheet;

class GhostsFactory
{
public:

    GhostsFactory() = default;
    GhostsFactory(const GhostsFactory&) = delete;
    ~GhostsFactory() = default;

    GhostsFactory& operator= (const GhostsFactory&) = delete;

    std::shared_ptr<Ghost> CreateGhost(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr,
                                       const GhostId ghostId) const;
};

} // Pacman namespace