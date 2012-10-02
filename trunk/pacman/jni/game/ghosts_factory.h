#pragma once

#include <cstdint>
#include <memory>

#include "base.h"
#include "game_listeners.h"

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

    std::shared_ptr<Ghost> CreateGhost(const GameLoader& loader, const Size actorSize, const std::shared_ptr<Map>& map,
                                       const std::weak_ptr<SpriteSheet>& spriteSheetPtr, const std::shared_ptr<IActorListener>& listener,
                                       const size_t ghostId) const;
};

} // Pacman namespace