#include "pacman_controller.h"

#include "error.h"
#include "math/math.h"
#include "actor.h"
#include "loader.h"
#include "spritesheet.h"
#include "frame_animator.h"

namespace Pacman {

static const std::string& kActorFileName = "pacman.json";
static const uint64_t kAnimationFrameDuration = 55;

static FORCEINLINE Rotation GetDirectionRotation(const MoveDirection direction)
{
    switch (direction)
    {
    case MoveDirection::Left:
        return Rotation(0.0f, 0.0f, Math::DegreesToRadians(180.0f));
    case MoveDirection::Right:
        return Rotation(0.0f, 0.0f, 0.0f);
    case MoveDirection::Up:
        return Rotation(0.0f, 0.0f, Math::DegreesToRadians(90.0f));
    case MoveDirection::Down:
        return Rotation(0.0f, 0.0f, Math::DegreesToRadians(270.0f));
    default:
        return Rotation(0.0f, 0.0f, 0.0f);
    }
}

static std::shared_ptr<FrameAnimator> MakeAnimator(const std::weak_ptr<SpriteSheet> spriteSheetPtr, const uint16_t actorSize)
{
    static const size_t kFramesCount = 4;
    const SpriteRegion region(0, 0, actorSize, actorSize);
    const std::shared_ptr<SpriteSheet> spriteSheet = spriteSheetPtr.lock();
    PACMAN_CHECK_ERROR(spriteSheet != nullptr, ErrorCode::BadArgument);

    const std::shared_ptr<Sprite> sprite_pacman_0 = spriteSheet->MakeSprite("pacman_anim_0", region);
    const std::shared_ptr<Sprite> sprite_pacman_1 = spriteSheet->MakeSprite("pacman_anim_1", region);
    const std::shared_ptr<Sprite> sprite_pacman_2 = spriteSheet->MakeSprite("pacman_anim_2", region);

    std::vector<std::shared_ptr<Sprite>> frames;
    frames.reserve(kFramesCount);
    frames.push_back(sprite_pacman_0);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_2);
    frames.push_back(sprite_pacman_1);

    return std::make_shared<FrameAnimator>(frames, kAnimationFrameDuration);
}

class ActorListener : public IActorListener
{
public:

    ActorListener(const std::shared_ptr<FrameAnimator> animator)
        : mAnimator(animator)
    {}

    ActorListener(const ActorListener&) = delete;
    ~ActorListener() = default;

    ActorListener& operator= (const ActorListener&) = delete;

    void OnDirectionChanged(const std::weak_ptr<Actor> actorPtr, const MoveDirection newDirection)
    {
        std::shared_ptr<Actor> actor = actorPtr.lock();
        PACMAN_CHECK_ERROR(actor != nullptr, ErrorCode::InvalidState);
        mAnimator->Resume();
        actor->Rotate(GetDirectionRotation(newDirection));
    }

    void OnTargetAchieved(const std::weak_ptr<Actor> actorPtr)
    {
        mAnimator->Pause();
    }

private:

    const std::shared_ptr<FrameAnimator> mAnimator;
};

PacmanController::PacmanController(const GameLoader& loader, const uint16_t actorSize, const std::shared_ptr<Map> map,
                                   const std::weak_ptr<SpriteSheet> spriteSheetPtr)
{
    mActorAnimator = MakeAnimator(spriteSheetPtr, actorSize);
    std::shared_ptr<ActorListener> listener = std::make_shared<ActorListener>(mActorAnimator);
    mActor = loader.LoadActor(kActorFileName, actorSize, mActorAnimator, map, listener);
}

void PacmanController::Update(const uint64_t dt)
{
    mActor->Update(dt);
    mActorAnimator->Update(dt);
}

void PacmanController::ChangeDirection(const MoveDirection newDirection)
{
    mActor->Move(newDirection, Actor::kMax);
}

} // Pacman namespace