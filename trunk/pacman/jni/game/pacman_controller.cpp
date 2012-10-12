#include "pacman_controller.h"

#include "error.h"
#include "math/math.h"
#include "common.h"
#include "game.h"
#include "actor.h"
#include "loader.h"
#include "map.h"
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

static std::shared_ptr<FrameAnimator> MakeAnimator(const std::weak_ptr<SpriteSheet>& spriteSheetPtr, const Size actorSize)
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

PacmanController::PacmanController(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
{
    mActorAnimator = MakeAnimator(spriteSheetPtr, actorSize);
    mActor = GetGame().GetLoader().LoadActor(kActorFileName, actorSize, mActorAnimator);
    ChangeDirection(mActor->GetDirection());
    OnDirectionChanged(mActor->GetDirection());
}

void PacmanController::Update(const uint64_t dt)
{
    mActor->Update(dt, *this);
    mActorAnimator->Update(dt);
}

void PacmanController::ChangeDirection(const MoveDirection newDirection)
{
    if (!CheckPassability(newDirection))
        return;

    // cornering (see Dosier guide)
    const CellIndexArray actorCells = GetGame().GetMap().FindCells(mActor->GetRegion());
    const CellIndex currentCell = SelectNearestCell(actorCells, newDirection);
    mActor->TranslateToCell(currentCell);

    const CellIndex targetCell = mActor->FindMaxAvailableCell(newDirection);
    mActor->MoveTo(newDirection, targetCell);
}

void PacmanController::TranslateTo(const CellIndex cell)
{
    mActor->TranslateToCell(cell);
    ChangeDirection(mActor->GetDirection());
}

void PacmanController::OnDirectionChanged(const MoveDirection newDirection)
{
    mActorAnimator->Resume();
    mActor->Rotate(GetDirectionRotation(newDirection));
}

void PacmanController::OnTargetAchieved()
{
    mActorAnimator->Pause();
}

bool PacmanController::CheckPassability(const MoveDirection direction) const
{
    Map& map = GetGame().GetMap();
    const CellIndexArray actorCells = map.FindCells(mActor->GetRegion());
    const CellIndex nearestCell = SelectNearestCell(actorCells, direction);
    const CellIndex nextCell = GetNext(nearestCell, direction);
    return (map.GetCell(nearestCell) == MapCellType::Empty) && (map.GetCell(nextCell) == MapCellType::Empty);
}

} // Pacman namespace