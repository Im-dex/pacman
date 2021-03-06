#include "ghost.h"

#include "error.h"
#include "sprite.h"
#include "spritesheet.h"
#include "drawable.h"
#include "actor.h"

namespace Pacman {

Ghost::Ghost(std::unique_ptr<Actor> actor, const Size size,
             const SpriteSheet& spriteSheet, const GhostState startState,
             const std::string& leftDrawableName, const std::string& rightDrawableName,
             const std::string& topDrawableName, const std::string& bottomDrawableName)
     : mStartState(startState),
       mState(startState),
       mActor(std::move(actor))
{
    const SpriteRegion region(0, 0, size, size);
    mLeftSprite = spriteSheet.MakeSprite(leftDrawableName, region);
    mRightSprite = spriteSheet.MakeSprite(rightDrawableName, region);
    mTopSprite = spriteSheet.MakeSprite(topDrawableName, region);
    mBottomSprite = spriteSheet.MakeSprite(bottomDrawableName, region);

    mActor->SetDrawable(mLeftSprite);
}

Ghost::~Ghost()
{
}

std::shared_ptr<IDrawable> Ghost::GetLeftDrawable() const
{
    return mLeftSprite;
}

std::shared_ptr<IDrawable> Ghost::GetRightDrawable() const
{
    return mRightSprite;
}

std::shared_ptr<IDrawable> Ghost::GetTopDrawable() const
{
    return mTopSprite;
}

std::shared_ptr<IDrawable> Ghost::GetBottomDrawable() const
{
    return mBottomSprite;
}

} // Pacman namespace