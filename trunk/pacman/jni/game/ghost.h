#pragma once

#include <memory>

#include "game_forwdecl.h"
#include "game_typedefs.h"

namespace Pacman {

class Ghost
{
public:

    Ghost(std::unique_ptr<Actor> actor, const Size size,
          const SpriteSheet& spriteSheet, const GhostState startState,
          const std::string& leftDrawableName, const std::string& rightDrawableName,
          const std::string& topDrawableName, const std::string& bottomDrawableName);

    Ghost(const Ghost&) = delete;
    virtual ~Ghost();

    Ghost& operator= (const Ghost&) = delete;

    virtual CellIndex SelectTargetCell() const = 0;

    std::shared_ptr<IDrawable> GetLeftDrawable() const;

    std::shared_ptr<IDrawable> GetRightDrawable() const;

    std::shared_ptr<IDrawable> GetTopDrawable() const;

    std::shared_ptr<IDrawable> GetBottomDrawable() const;

    GhostState GetState() const
    {
        return mState;
    }

    GhostState GetStartState() const
    {
        return mStartState;
    }

    void SetState(const GhostState state)
    {
        mState = state;
    }

    Actor& GetActor() const
    {
        return *mActor;
    }

protected:

    const GhostState        mStartState;
    GhostState              mState;
    std::unique_ptr<Actor>  mActor;
    std::shared_ptr<Sprite> mLeftSprite;
    std::shared_ptr<Sprite> mRightSprite;
    std::shared_ptr<Sprite> mTopSprite;
    std::shared_ptr<Sprite> mBottomSprite;
};

} // Pacman namespace