#include "actor.h"

#include "error.h"
#include "scene_node.h"
#include "scene_manager.h"
#include "json_helper.h"
#include "map.h"

namespace Pacman {

static FORCEINLINE SpritePosition CalcActorPivotOffset(const uint16_t size)
{
    const uint16_t sizeHalf = size / 2;
    return SpritePosition(sizeHalf, sizeHalf);
}

const uint16_t Actor::kMax = uint16_t(-1);

Actor::Actor(const uint16_t size, const uint16_t speed, const uint16_t cellSize,
             const SpritePosition& startPosition, const std::shared_ptr<IDrawable> startDrawable,
             const std::shared_ptr<Map> map, const std::shared_ptr<IActorListener> listener)
     : mSize(size),
       mSpeed(speed),
       mCellSize(cellSize),
       mCenterOffset(CalcActorPivotOffset(size)),
       mMap(map),
       mListener(listener),
       mMoveTarget(0, 0),
       mDirection(MoveDirection::None),
       mNode(std::make_shared<SceneNode>(startDrawable, CalcActorPivotOffset(size), startPosition, Rotation::kZero))
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

void Actor::Update(const uint64_t dt)
{
    const int32_t offset = static_cast<const int32_t>(mSpeed * mCellSize * dt / 1000);

    const SpritePosition currentPosition = GetCenterPos();
    const int32_t xDiff = static_cast<const int32_t>(mMoveTarget.GetX()) - currentPosition.GetX();
    const int32_t yDiff = static_cast<const int32_t>(mMoveTarget.GetY()) - currentPosition.GetY();

    const int32_t xOffset = ((xDiff < 0) ? -1 : 1) * (std::min(offset, std::abs(xDiff)));
    const int32_t yOffset = ((yDiff < 0) ? -1 : 1) * (std::min(offset, std::abs(yDiff)));

    if ((xOffset != 0) || (yOffset != 0))
    {
        mNode->Move(xOffset, yOffset);
    }
    else
    {
        if (mListener != nullptr)
            mListener->OnTargetAchieved(shared_from_this());
    }
}

SpritePosition Actor::GetCenterPos() const
{
    return mNode->GetPosition() + mCenterOffset;
}

SpriteRegion Actor::GetRegion() const
{
    return SpriteRegion(mNode->GetPosition(), mSize, mSize);
}

void Actor::Rotate(const Rotation& rotation)
{
    mNode->Rotate(rotation);
}

void Actor::Translate(const CellIndex& cell)
{
    PACMAN_CHECK_ERROR(mMap->GetCell(cell) == MapCellType::Empty, ErrorCode::BadArgument);
    mNode->Translate(mMap->GetCellCenterPos(cell) - mCenterOffset);
}

void Actor::Move(const MoveDirection direction, const uint16_t cellsCount)
{
    CellIndex targetCell(0, 0);
    const CellIndex currentCell = mMap->FindCell(GetRegion());

    if (cellsCount == kMax)
    {
        targetCell = FindMaxAvailableCell(currentCell, direction);
    }
    else
    {
        switch (direction)
        {
        case MoveDirection::Left:
            targetCell = currentCell - CellIndex(cellsCount, 0);
            break;
        case MoveDirection::Right:
            targetCell = currentCell + CellIndex(cellsCount, 0);
            break;
        case MoveDirection::Up:
            targetCell = currentCell - CellIndex(0, cellsCount);
            break;
        case MoveDirection::Down:
            targetCell = currentCell + CellIndex(0, cellsCount);
            break;
        default:
            targetCell = currentCell;
        }
    }

    PACMAN_CHECK_ERROR(mMap->GetCell(targetCell) == MapCellType::Empty, ErrorCode::BadArgument);

    if (targetCell == currentCell) // no way
        return;

    mMoveTarget = mMap->GetCellCenterPos(targetCell);
    mDirection = direction;
    if (mListener != nullptr)
        mListener->OnDirectionChanged(shared_from_this(), direction);
}

void Actor::SetDrawable(const std::shared_ptr<IDrawable> drawable)
{
    mNode->SetDrawable(drawable);
}

CellIndex Actor::FindMaxAvailableCell(const CellIndex& curCell, const MoveDirection direction) const
{
    CellIndex index = curCell;
    bool canMove = true;

    while (canMove)
    {
        const MapNeighborsInfo neighbors = mMap->GetDirectNeighbors(index);

        switch (direction)
        {
        case MoveDirection::Left:
            {
                if (neighbors.left == MapCellType::Empty)
                    index.SetY(index.GetY() - 1);
                else
                    canMove = false;
                break;
            }
        case MoveDirection::Right:
            {
                if (neighbors.right == MapCellType::Empty)
                    index.SetY(index.GetY() + 1);
                else
                    canMove = false;
                break;
            }
        case MoveDirection::Up:
            {
                if (neighbors.top == MapCellType::Empty)
                    index.SetX(index.GetX() - 1);
                else
                    canMove = false;
                break;
            }
        case MoveDirection::Down:
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