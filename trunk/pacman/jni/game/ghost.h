#pragma once

#include <memory>

#include "game_typedefs.h"

namespace Pacman {

class IDrawable;
class Sprite;
class SpriteSheet;
class Actor;

class Ghost
{
public:

    Ghost(const std::shared_ptr<Actor>& actor, const Size size,
          const std::weak_ptr<SpriteSheet>& spriteSheetPtr,
          const std::string& leftDrawableName, const std::string& rightDrawableName,
          const std::string& topDrawableName, const std::string& bottomDrawableName);

    Ghost(const Ghost&) = delete;
    virtual ~Ghost() {}

    Ghost& operator= (const Ghost&) = delete;

    virtual void SelectTargetCell() const = 0;

    std::shared_ptr<IDrawable> GetLeftDrawable() const;

    std::shared_ptr<IDrawable> GetRightDrawable() const;

    std::shared_ptr<IDrawable> GetTopDrawable() const;

    std::shared_ptr<IDrawable> GetBottomDrawable() const;

    GhostState GetState() const
    {
        return mState;
    }

    void SetState(const GhostState state)
    {
        mState = state;
    }

    std::shared_ptr<Actor> GetActor() const
    {
        return mActor;
    }

private:

    GhostState mState;
    std::shared_ptr<Actor>  mActor;
    std::shared_ptr<Sprite> mLeftSprite;
    std::shared_ptr<Sprite> mRightSprite;
    std::shared_ptr<Sprite> mTopSprite;
    std::shared_ptr<Sprite> mBottomSprite;
};

} // Pacman namespace