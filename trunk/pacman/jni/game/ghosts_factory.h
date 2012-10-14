#pragma once

#include <cstdint>
#include <memory>

#include "base.h"
#include "game_forwdecl.h"
#include "game_typedefs.h"

namespace Pacman {

class GhostsFactory
{
public:

    GhostsFactory() = default;
    GhostsFactory(const GhostsFactory&) = delete;
    ~GhostsFactory() = default;

    GhostsFactory& operator= (const GhostsFactory&) = delete;

    std::unique_ptr<Ghost> CreateGhost(const Size actorSize, const SpriteSheet& spriteSheet,
                                       const GhostId ghostId) const;
};

} // Pacman namespace