#include "actor.h"

#include <algorithm>

#include "error.h"
#include "common.h"
#include "game.h"
#include "actor_controller.h"
#include "scene_node.h"
#include "scene_manager.h"
#include "json_helper.h"
#include "map.h"

namespace Pacman {

static FORCEINLINE Position CalcActorPivotOffset(const Size size)
{
    const Size sizeHalf = size / 2;
    return Position(sizeHalf, sizeHalf);
}

static FORCEINLINE Position CalcTargetPosition(const MoveDirection direction, const Position& current,
                                               const Position& target)
{
    switch (direction)
    {
    case MoveDirection::Left:
    case MoveDirection::Right:
        return Position(target.GetX(), current.GetY());
    case MoveDirection::Up:
    case MoveDirection::Down:
        return Position(current.GetX(), target.GetY());
    default:
        return current;
    }
}

Actor::Actor(const Size size, const Speed speed, const Position& startPosition,
             const MoveDirection startDirection, const std::shared_ptr<IDrawable>& startDrawable)
     : mSize(size),
       mSpeed(speed),
       mPivotOffset(CalcActorPivotOffset(size)),
       mStartPosition(startPosition + mPivotOffset),
       mStartDirection(startDirection),
       mMoveTarget(Position::kZero),
       mDirection(startDirection),
       mNode(std::make_shared<SceneNode>(startDrawable, startPosition, Rotation::kZero)),
       mDirectionChanged(false)
{
}

void Actor::AttachToScene(SceneManager& sceneManager) const
{
    sceneManager.AttachNode(mNode);
}

void Actor::DetachFromScene(SceneManager& sceneManager) const
{
    sceneManager.DetachNode(mNode);
}

void Actor::Update(const uint64_t dt, IActorController& controller)
{
    const Size cellSize = GetGame().GetMap().GetCellSize();
    const float accurateOffset = ((static_cast<float>(dt) * 0.001f) * static_cast<float>(mSpeed)) * static_cast<float>(cellSize);
    const PosOffset offset = static_cast<PosOffset>(accurateOffset);

    DoMove(controller, offset, false);
}

void Actor::DoMove(IActorController& controller, const PosOffset offset,
                   const bool recursive)
{
    if (mDirectionChanged)
    {
        controller.OnDirectionChanged(mDirection);
        mDirectionChanged = false;
    }

    const Position currentPosition = GetCenterPos();
    const PosOffset xDiff = static_cast<PosOffset>(mMoveTarget.GetX()) - currentPosition.GetX();
    const PosOffset yDiff = static_cast<PosOffset>(mMoveTarget.GetY()) - currentPosition.GetY();
    const PosOffset absXDiff = std::abs(xDiff);
    const PosOffset absYDiff = std::abs(yDiff);

    const PosOffset xOffset = ((xDiff < 0) ? -1 : 1) * (std::min(offset, absXDiff));
    const PosOffset yOffset = ((yDiff < 0) ? -1 : 1) * (std::min(offset, absYDiff));

    mNode->Move(xOffset, yOffset);

    // if not full move
    if ((offset > absXDiff) && (offset > absYDiff))
    {
        if (recursive)
            return;

        controller.OnTargetAchieved();
        const PosOffset reduceValue = std::max(absXDiff, absYDiff);
        DoMove(controller, offset - reduceValue, true);
        return;
    }
}

Position Actor::GetCenterPos() const
{
    return mNode->GetPosition() + mPivotOffset;
}

SpriteRegion Actor::GetRegion() const
{
    return SpriteRegion(mNode->GetPosition(), mSize, mSize);
}

void Actor::Rotate(const Rotation& rotation)
{
    mNode->SetRotation(rotation, mPivotOffset);
}

void Actor::TranslateToCell(const CellIndex& cell)
{
    Map& map = GetGame().GetMap();
    PACMAN_CHECK_ERROR(map.GetCell(cell) == MapCellType::Empty);
    TranslateToPosition(map.GetCellCenterPos(cell));
}

void Actor::TranslateToPosition(const Position& position)
{
    mMoveTarget = position;
    mNode->Translate(mMoveTarget - mPivotOffset);
}

void Actor::Move(const MoveDirection direction, const Size wayLength)
{
    if (mDirection != direction)
    {
        mDirection = direction;
        mDirectionChanged = true;
    }

    const Position targetPosition = GetFuturePosition(GetCenterPos(), direction, wayLength);
    mMoveTarget = CalcTargetPosition(direction, GetCenterPos(), targetPosition);
}

void Actor::MoveTo(const MoveDirection direction, const CellIndex& cell)
{
    Map& map = GetGame().GetMap();
    PACMAN_CHECK_ERROR(map.GetCell(cell) == MapCellType::Empty);

    if (mDirection != direction)
    {
        mDirection = direction;
        mDirectionChanged = true;
    }

    const Position targetPosition = map.GetCellCenterPos(cell);
    mMoveTarget = CalcTargetPosition(direction, GetCenterPos(), targetPosition);
}

CellIndex Actor::FindMaxAvailableCell(const MoveDirection direction) const
{
    const CellIndexArray cells = GetGame().GetMap().FindCells(GetRegion());
    CellIndex cellIndex = SelectNearestCell(cells, direction);
    bool canMove = true;

    while (canMove)
    {
        const MapNeighborsInfo neighbors = GetGame().GetMap().GetDirectNeighbors(cellIndex);
        for (const Neighbor& neighbor : neighbors.mNeighbors)
        {
            if (neighbor.mDirection != direction)
                continue;

            if ((neighbor.mCellType == MapCellType::Empty) ||
                ((neighbor.mCellType == MapCellType::Door) && (neighbor.mDirection == MoveDirection::Up))) // door is passable from bottom to top
            {
                cellIndex = GetNext(cellIndex, neighbor.mDirection);
            }
            else
            {
                canMove = false;
            }
            break;
        }
    }

    return cellIndex;
}

void Actor::SetDrawable(const std::shared_ptr<IDrawable>& drawable)
{
    mNode->SetDrawable(drawable);
}

} // Pacman namespace