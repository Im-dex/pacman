#pragma once

#include <memory>
#include <cstdint>

#include "base.h"
#include "game_listeners.h"

namespace Pacman {

class Actor;
class GameLoader;
class SpriteSheet;
class Map;
class FrameAnimator;

class PacmanController
{
public:

    PacmanController(const GameLoader& loader, const Size actorSize, const std::shared_ptr<Map>& map,
                     const std::weak_ptr<SpriteSheet>& spriteSheetPtr);
    PacmanController(const PacmanController&) = delete;
    ~PacmanController() = default;

    PacmanController& operator= (const PacmanController&) = delete;

    void Update(const uint64_t dt);

    void ChangeDirection(const MoveDirection newDirection);

    std::shared_ptr<Actor> GetActor() const
    {
        return mActor;
    }

private:

    std::shared_ptr<Actor>         mActor;
    std::shared_ptr<FrameAnimator> mActorAnimator;
};

} // Pacman namespace