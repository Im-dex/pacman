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

//static FORCEINLINE CellIndex FindCellWithOffset(const CellIndex& currentCellIndex, const MoveDirection direction,
//                                                const CellIndex::value_t indexOffset)
//{
//    switch (direction)
//    {
//    case MoveDirection::Left:
//        return currentCellIndex - CellIndex(0, indexOffset);
//    case MoveDirection::Right:
//        return currentCellIndex + CellIndex(0, indexOffset);
//    case MoveDirection::Up:
//        return currentCellIndex - CellIndex(indexOffset, 0);
//    case MoveDirection::Down:
//        return currentCellIndex + CellIndex(indexOffset, 0);
//    default:
//        return currentCellIndex;
//    }
//}

Actor::Actor(const Size size, const Speed speed, const Position& startPosition,
             const MoveDirection startDirection, const std::shared_ptr<IDrawable>& startDrawable)
     : mSize(size),
       mSpeed(speed),
       mPivotOffset(CalcActorPivotOffset(size)),
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

void Actor::Update(const uint64_t dt, IActorController* controller)
{
    const Size cellSize = GetGame().GetMap().GetCellSize();

    if (mDirectionChanged)
    {
        if (controller != nullptr)
            controller->OnDirectionChanged(mDirection);
        mDirectionChanged = false;
    }

    const float accurateOffset = ((static_cast<float>(dt) * 0.001f) * static_cast<float>(mSpeed)) * static_cast<float>(cellSize);
    const PosOffset offset = static_cast<PosOffset>(accurateOffset);

    const Position currentPosition = GetCenterPos();
    const PosOffset xDiff = static_cast<PosOffset>(mMoveTarget.GetX()) - currentPosition.GetX();
    const PosOffset yDiff = static_cast<PosOffset>(mMoveTarget.GetY()) - currentPosition.GetY();

    const PosOffset xOffset = ((xDiff < 0) ? -1 : 1) * (std::min(offset, std::abs(xDiff)));
    const PosOffset yOffset = ((yDiff < 0) ? -1 : 1) * (std::min(offset, std::abs(yDiff)));

    if ((xOffset != 0) || (yOffset != 0))
    {
        mNode->Move(xOffset, yOffset);
    }
    else
    {
        if (controller != nullptr)
            controller->OnTargetAchieved();
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
    PACMAN_CHECK_ERROR(map.GetCell(cell) == MapCellType::Empty, ErrorCode::BadArgument);
    mNode->Translate(map.GetCellCenterPos(cell) - mPivotOffset);
}

void Actor::TranslateToPosition(const Position& position)
{
    mNode->Translate(position);
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
    PACMAN_CHECK_ERROR(map.GetCell(cell) == MapCellType::Empty, ErrorCode::BadArgument);

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

//void Actor::Move(const MoveDirection direction, const CellIndex::value_t cellsCount, const bool hasCornering)
//{
//    Map& map = GetGame().GetMap();
//    const CellIndexArray currentCells = map.FindCells(GetRegion());
//    const CellIndex currentCell = SelectNearestCell(currentCells, direction);
//    const CellIndex targetCell = (cellsCount == kMax) ? FindMaxAvailableCell(currentCell, direction)
//                                                      : FindCellWithOffset(currentCell, direction, cellsCount);
//
//    PACMAN_CHECK_ERROR(map.GetCell(targetCell) == MapCellType::Empty, ErrorCode::BadArgument);
//    if (std::find(currentCells.begin(), currentCells.end(), targetCell) != currentCells.end())
//        return; // no way
//
//    // ONLY FOR PACMAN ACTOR!!!
//    // when pacman change move direction on a crossroads, move him to the current cell center.
//    // works only if pacman change direction to the perpendicular of current.
//    // ("cornering", see Dosier guide)
//    if (hasCornering && (mDirection != direction) && (GetBackDirection(mDirection) != direction))
//        TranslateTo(currentCell);
//
//    // calc next movement
//    const Position targetCellCenterPos = map.GetCellCenterPos(targetCell);
//    const Position currentCenterPos = GetCenterPos();
//    switch (direction)
//    {
//    case MoveDirection::Left:
//        {
//            const Size distance = currentCenterPos.GetX() - targetCellCenterPos.GetX();
//            mMoveTarget = currentCenterPos - Position(distance, 0);
//            break;
//        }
//    case MoveDirection::Right:
//        {
//            const Size distance = targetCellCenterPos.GetX() - currentCenterPos.GetX();
//            mMoveTarget = currentCenterPos + Position(distance, 0);
//            break;
//        }
//    case MoveDirection::Up:
//        {
//            const Size distance = currentCenterPos.GetY() - targetCellCenterPos.GetY();
//            mMoveTarget = currentCenterPos - Position(0, distance);
//            break;
//        }
//    case MoveDirection::Down:
//        {
//            const Size distance = targetCellCenterPos.GetY() - currentCenterPos.GetY();
//            mMoveTarget = currentCenterPos + Position(0, distance);
//            break;
//        }
//    default:
//        mMoveTarget = currentCenterPos;
//    }
//
//    mDirection = direction;
//    mDirectionChanged = true;
//}

void Actor::SetDrawable(const std::shared_ptr<IDrawable>& drawable)
{
    mNode->SetDrawable(drawable);
}

} // Pacman namespace