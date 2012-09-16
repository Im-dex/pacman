#include "pacman.h"

#include "error.h"
#include "math/math.h"
#include "spritesheet.h"
#include "scene_node.h"
#include "scene_manager.h"
#include "frame_animator.h"
#include "map.h"

namespace Pacman {

static FORCEINLINE Rotation GetDirectionRotation(const PacmanMoveDirection direction)
{
    switch (direction)
    {
    case PacmanMoveDirection::Left:
        return Rotation(0.0f, 0.0f, Math::DegreesToRadians(180.0f));
    case PacmanMoveDirection::Right:
        return Rotation(0.0f, 0.0f, 0.0f);
    case PacmanMoveDirection::Up:
        return Rotation(0.0f, 0.0f, Math::DegreesToRadians(90.0f));
    case PacmanMoveDirection::Down:
        return Rotation(0.0f, 0.0f, Math::DegreesToRadians(270.0f));
    default:
        return Rotation(0.0f, 0.0f, 0.0f);
    }
}

static std::shared_ptr<FrameAnimator> MakeAnimator(const std::weak_ptr<SpriteSheet> spriteSheet, const uint64_t animationFrameDuration,
                                                   const uint16_t actorSize)
{
    static const size_t kFramesCount = 4;
    const SpriteRegion region(0, 0, actorSize, actorSize);
    const std::shared_ptr<SpriteSheet> spriteSheetPtr = spriteSheet.lock();
    PACMAN_CHECK_ERROR(spriteSheetPtr != nullptr, ErrorCode::BadArgument);

    const std::shared_ptr<Sprite> sprite_pacman_0 = spriteSheetPtr->MakeSprite("pacman_anim_0", region);
    const std::shared_ptr<Sprite> sprite_pacman_1 = spriteSheetPtr->MakeSprite("pacman_anim_1", region);
    const std::shared_ptr<Sprite> sprite_pacman_2 = spriteSheetPtr->MakeSprite("pacman_anim_2", region);

    std::vector<std::shared_ptr<Sprite>> frames;
    frames.reserve(kFramesCount);
    frames.push_back(sprite_pacman_0);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_2);
    frames.push_back(sprite_pacman_1);
    //frames.push_back(sprite_pacman_0);

    return std::make_shared<FrameAnimator>(frames, animationFrameDuration);
}

PacmanActor::PacmanActor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
                         const uint16_t cellSize, const uint64_t animationFrameDuration,
                         const PacmanMoveDirection startDirection, const std::weak_ptr<SpriteSheet> spriteSheet,
                         const std::shared_ptr<Map> map)
           : Actor(size, speed, startPosition, cellSize, nullptr),
             mSpriteCenterOffset(size / 2, size / 2),
             mAnimator(MakeAnimator(spriteSheet, animationFrameDuration, size)),
             mDirection(PacmanMoveDirection::None),
             mMap(map)
{
    Actor::mNode->SetDrawable(mAnimator);
    ChangeDirection(startDirection);
}

void PacmanActor::ChangeDirection(const PacmanMoveDirection direction)
{
    const CellIndex index = mMap->FindCell(GetRegion());
    const CellIndex maxAvailableCellIndex = FindMaxAvailableCell(index, direction);
    if (index != maxAvailableCellIndex)
    {
        SpritePosition maxAvailablePos = mMap->GetCellCenterPos(maxAvailableCellIndex);
        maxAvailablePos -= mSpriteCenterOffset; // because sprite pos is the left top point, not center
        Rotate(GetDirectionRotation(direction));
        MoveTo(maxAvailablePos);
        mDirection = direction;
    }
}

void PacmanActor::TranslateTo(const CellIndex& index)
{
    const SpritePosition position = mMap->GetCellCenterPos(index) - mSpriteCenterOffset;
    Translate(position);
    ChangeDirection(mDirection);
}

void PacmanActor::Update(const uint64_t dt)
{
    Actor::Update(dt);
    mAnimator->Update(dt);
}

CellIndex PacmanActor::FindMaxAvailableCell(const CellIndex& curCellIndex, const PacmanMoveDirection direction) const
{
    CellIndex index = curCellIndex;
    bool canMove = true;

    while (canMove)
    {
        const MapNeighborsInfo neighbors = mMap->GetDirectNeighbors(index);

        switch (direction)
        {
        case PacmanMoveDirection::Left:
            {
                if (neighbors.left == MapCellType::Empty)
                    index.SetY(index.GetY() - 1);
                else
                    canMove = false;
                break;
            }
        case PacmanMoveDirection::Right:
            {
                if (neighbors.right == MapCellType::Empty)
                    index.SetY(index.GetY() + 1);
                else
                    canMove = false;
                break;
            }
        case PacmanMoveDirection::Up:
            {
                if (neighbors.top == MapCellType::Empty)
                    index.SetX(index.GetX() - 1);
                else
                    canMove = false;
                break;
            }
        case PacmanMoveDirection::Down:
            {
                if (neighbors.bottom == MapCellType::Empty)
                    index.SetX(index.GetX() + 1);
                else
                    canMove = false;
                break;
            }
        default: // none
            canMove = false;
        }
    }

    return index;
}

} // Pacman namespace