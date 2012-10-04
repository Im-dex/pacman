#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <cassert>

#include "base.h"
#include "actor_controller.h"

namespace Pacman {

class Actor;
class Ghost;
class GameLoader;
class SpriteSheet;
class Map;

class AIController : public IActorController
{
public:

    static const size_t kGhostsCount = 4;

    AIController(const GameLoader& loader, const Size actorSize, const std::shared_ptr<Map>& map,
                 const std::weak_ptr<SpriteSheet>& spriteSheetPtr);
    AIController(const AIController&) = delete;
    ~AIController() = default;

    AIController& operator= (const AIController&) = delete;

    void Update(const uint64_t dt);

    std::shared_ptr<Actor> GetActor(const size_t index) const;

    virtual void OnDirectionChanged(const MoveDirection newDirection);

    virtual void OnTargetAchieved();

private:

    typedef std::array<std::shared_ptr<Ghost>, kGhostsCount> GhostsArray; 

    GhostsArray mGhosts;
};

} // Pacman namespace