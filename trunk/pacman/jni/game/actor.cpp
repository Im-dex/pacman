#include "actor.h"

#include <algorithm>

#include "error.h"
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

// if cells count greater than 1 select nearest cell for the current direction
// (for example: if direction is left, select one of the most left placed cells)
static CellIndex SelectNearestCell(const CellIndexArray& currentCellsIndices, const MoveDirection direction)
{
    CellIndex result = currentCellsIndices[0];

    for (size_t i = 1; i < currentCellsIndices.size(); i++)
    {
        const CellIndex& cellIndex = currentCellsIndices[i];
        switch (direction)
        {
        case MoveDirection::Left:
            if (GetColumn(cellIndex) < GetColumn(result))
                result = cellIndex;
            break;
        case MoveDirection::Right:
            if (GetColumn(cellIndex) > GetColumn(result))
                result = cellIndex;
            break;
        case MoveDirection::Up:
            if (GetRow(cellIndex) < GetRow(result))
                result = cellIndex;
            break;
        case MoveDirection::Down:
            if (GetRow(cellIndex) > GetRow(result))
                result = cellIndex;
            break;
        }
    }

    return result;
}

static FORCEINLINE CellIndex FindCellWithOffset(const CellIndex& currentCellIndex, const MoveDirection direction,
                                                const CellIndex::value_t indexOffset)
{
    switch (direction)
    {
    case MoveDirection::Left:
        return currentCellIndex - CellIndex(0, indexOffset);
    case MoveDirection::Right:
        return currentCellIndex + CellIndex(0, indexOffset);
    case MoveDirection::Up:
        return currentCellIndex - CellIndex(indexOffset, 0);
    case MoveDirection::Down:
        return currentCellIndex + CellIndex(indexOffset, 0);
    default:
        return currentCellIndex;
    }
}

const FORCEINLINE MoveDirection GetBackDirection(const MoveDirection direction)
{
    switch (direction)
    {
    case MoveDirection::Left:
        return MoveDirection::Right;
    case MoveDirection::Right:
        return MoveDirection::Left;
    case MoveDirection::Up:
        return MoveDirection::Down;
    case MoveDirection::Down:
        return MoveDirection::Up;
    default:
        return MoveDirection::None;
    }
}

const CellIndex::value_t Actor::kMax = 0;

Actor::Actor(const Size size, const Speed speed, const Size cellSize,
             const Position& startPosition, const std::shared_ptr<IDrawable>& startDrawable,
             const std::shared_ptr<Map>& map)
     : mSize(size),
       mSpeed(speed),
       mCellSize(cellSize),
       mPivotOffset(CalcActorPivotOffset(size)),
       mMap(map),
       mMoveTarget(Position::kZero),
       mDirection(MoveDirection::None),
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
    if (mDirectionChanged)
    {
        if (controller != nullptr)
            controller->OnDirectionChanged(mDirection);
        mDirectionChanged = false;
    }

    const float accurateOffset = ((static_cast<float>(dt) * 0.001f) * static_cast<float>(mSpeed)) * static_cast<float>(mCellSize);
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

void Actor::TranslateTo(const CellIndex& cell)
{
    PACMAN_CHECK_ERROR(mMap->GetCell(cell) == MapCellType::Empty, ErrorCode::BadArgument);
    mNode->Translate(mMap->GetCellCenterPos(cell) - mPivotOffset);
}

void Actor::Move(const MoveDirection direction, const CellIndex::value_t cellsCount)
{
    const CellIndexArray currentCells = mMap->FindCells(GetRegion());
    const CellIndex currentCell = SelectNearestCell(currentCells, direction);
    const CellIndex targetCell = (cellsCount == kMax) ? FindMaxAvailableCell(currentCell, direction)
                                                      : FindCellWithOffset(currentCell, direction, cellsCount);

    PACMAN_CHECK_ERROR(mMap->GetCell(targetCell) == MapCellType::Empty, ErrorCode::BadArgument);
    if (std::find(currentCells.begin(), currentCells.end(), targetCell) != currentCells.end())
        return; // no way

    // ONLY FOR PACMAN ACTOR!!! (only pacman uses kMax, we are can check him by this value)
    // when pacman change move direction on a crossroads, move him to the current cell center.
    // works only if pacman change direction to the perpendicular of current.
    // ("cornering", see Dosier guide)
    if ((cellsCount == kMax) && (mDirection != direction) && (GetBackDirection(mDirection) != direction))
        TranslateTo(currentCell);

    // calc next movement
    const Position targetCellCenterPos = mMap->GetCellCenterPos(targetCell);
    const Position currentCenterPos = GetCenterPos();
    switch (direction)
    {
    case MoveDirection::Left:
        {
            const Size distance = currentCenterPos.GetX() - targetCellCenterPos.GetX();
            mMoveTarget = currentCenterPos - Position(distance, 0);
            break;
        }
    case MoveDirection::Right:
        {
            const Size distance = targetCellCenterPos.GetX() - currentCenterPos.GetX();
            mMoveTarget = currentCenterPos + Position(distance, 0);
            break;
        }
    case MoveDirection::Up:
        {
            const Size distance = currentCenterPos.GetY() - targetCellCenterPos.GetY();
            mMoveTarget = currentCenterPos - Position(0, distance);
            break;
        }
    case MoveDirection::Down:
        {
            const Size distance = targetCellCenterPos.GetY() - currentCenterPos.GetY();
            mMoveTarget = currentCenterPos + Position(0, distance);
            break;
        }
    default:
        mMoveTarget = currentCenterPos;
    }

    mDirection = direction;
    mDirectionChanged = true;
}

void Actor::SetDrawable(const std::shared_ptr<IDrawable>& drawable)
{
    mNode->SetDrawable(drawable);
}

CellIndex Actor::FindMaxAvailableCell(const CellIndex& currentCellIndex, const MoveDirection direction) const
{
    CellIndex cellIndex = currentCellIndex;
    bool canMove = true;

    while (canMove)
    {
        const MapNeighborsInfo neighbors = mMap->GetDirectNeighbors(cellIndex);

        switch (direction)
        {
        case MoveDirection::Left:
            {
                if (neighbors.left == MapCellType::Empty)
                    SetColumn(cellIndex, GetColumn(cellIndex) - 1);
                else
                    canMove = false;
                break;
            }
        case MoveDirection::Right:
            {
                if (neighbors.right == MapCellType::Empty)
                    SetColumn(cellIndex, GetColumn(cellIndex) + 1);
                else
                    canMove = false;
                break;
            }
        case MoveDirection::Up:
            {
                if (neighbors.top == MapCellType::Empty)
                    SetRow(cellIndex, GetRow(cellIndex) - 1);
                else
                    canMove = false;
                break;
            }
        case MoveDirection::Down:
            {
                if (neighbors.bottom == MapCellType::Empty)
                    SetRow(cellIndex, GetRow(cellIndex) + 1);
                else
                    canMove = false;
                break;
            }
        default: // none
            canMove = false;
        }
    }

    return cellIndex;
}

} // Pacman namespace