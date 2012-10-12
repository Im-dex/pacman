#pragma once

#include <memory>
#include <cstdint>

#include "base.h"
#include "game_forwdecl.h"
#include "actor_controller.h"

namespace Pacman {

class PacmanController : public IActorController
{
public:

    PacmanController(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr);
    PacmanController(const PacmanController&) = delete;
    ~PacmanController() = default;

    PacmanController& operator= (const PacmanController&) = delete;

    void Update(const uint64_t dt);

    void ChangeDirection(const MoveDirection newDirection);

    void TranslateTo(const CellIndex cell);

    bool OnPacmanFail();

    void ResetState();

    virtual void OnDirectionChanged(const MoveDirection newDirection);

    virtual void OnTargetAchieved();

    std::shared_ptr<Actor> GetActor() const
    {
        return mActor;
    }

private:

    bool CheckPassability(const MoveDirection direction) const;

    std::shared_ptr<Actor>         mActor;
    std::shared_ptr<FrameAnimator> mActorAnimator;
    size_t                         mLivesCount;
};

} // Pacman namespace