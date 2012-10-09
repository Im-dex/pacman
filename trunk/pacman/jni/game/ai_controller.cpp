#include "ai_controller.h"

#include "log.h"
#include "common.h"
#include "math.h"
#include "game.h"
#include "actor.h"
#include "ghost.h"
#include "loader.h"
#include "error.h"
#include "spritesheet.h"
#include "map.h"
#include "ghosts_factory.h"
#include "pacman_controller.h"
#include "scheduler.h"
#include "dots_grid.h"

namespace Pacman {

static const CellIndex::value_t kWayLength = 1;

AIController::AIController(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
            : mAIInfo(GetGame().GetLoader().LoadAIInfo("ai.json")),
              mCurrentGhost(kGhostsCount)
{
    GhostsFactory factory;
    mGhosts[GhostsFactory::kBlinky] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostsFactory::kBlinky);
    mGhosts[GhostsFactory::kPinky] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostsFactory::kPinky);
    mGhosts[GhostsFactory::kInky] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostsFactory::kInky);
    mGhosts[GhostsFactory::kClyde] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostsFactory::kClyde);

    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        const std::shared_ptr<Actor> actor = ghost->GetActor();
        const CellIndex startTargetCell = actor->FindMaxAvailableCell(actor->GetDirection());
        actor->MoveTo(actor->GetDirection(), startTargetCell);
    }

    SetupScheduler();
}

void AIController::Update(const uint64_t dt)
{
    for (size_t i = 0; i < kGhostsCount; i++)
    {
        mCurrentGhost = i;
        mGhosts[i]->GetActor()->Update(dt, this);
    }
    //mCurrentGhost = GhostsFactory::kInky;
    //mGhosts[mCurrentGhost]->GetActor()->Update(dt, this);
}

std::shared_ptr<Actor> AIController::GetActor(const size_t index) const
{
    return mGhosts[index]->GetActor();
}

void AIController::OnDirectionChanged(const MoveDirection newDirection)
{
    const std::shared_ptr<Ghost> ghost = mGhosts[mCurrentGhost];

    switch (newDirection)
    {
    case MoveDirection::Left:
        ghost->GetActor()->SetDrawable(ghost->GetLeftDrawable());
        break;
    case MoveDirection::Right:
        ghost->GetActor()->SetDrawable(ghost->GetRightDrawable());
        break;
    case MoveDirection::Up:
        ghost->GetActor()->SetDrawable(ghost->GetTopDrawable());
        break;
    case MoveDirection::Down:
        ghost->GetActor()->SetDrawable(ghost->GetBottomDrawable());
        break;
    default:
        break;
    }
}

void AIController::OnTargetAchieved()
{
    switch (mGhosts[mCurrentGhost]->GetState())
    {
    case GhostState::Wait:
        FindWayOnWaitState();
        break;
    case GhostState::Chase:
        FindWayOnChaseState();
        break;
    case GhostState::Scatter:
        FindWayOnScatterState();
        break;
    case GhostState::Frightened:
        FindWayOnFrightenedState();
        break;
    default:
        PACMAN_CHECK_ERROR(false, ErrorCode::InvalidState);
    }
}

// move up & down
void AIController::FindWayOnWaitState()
{
    Map& map = GetGame().GetMap();
    const std::shared_ptr<Ghost> ghost = mGhosts[mCurrentGhost];
    const std::shared_ptr<Actor> actor = ghost->GetActor();
    const MoveDirection direction = actor->GetDirection();

    const CellIndex currentCell = SelectNearestCell(map.FindCells(actor->GetRegion()), direction);
    const CellIndex next = GetNext(currentCell, direction);
    if (map.GetCell(next) != MapCellType::Empty)
    {
        const MoveDirection backDirection = GetBackDirection(direction);
        const CellIndex newTargetCell = actor->FindMaxAvailableCell(backDirection);
        actor->MoveTo(backDirection, newTargetCell);
    }
}

// move to the nearest crossroad
void AIController::FindWayOnChaseState()
{
    const std::shared_ptr<Ghost> ghost = mGhosts[mCurrentGhost];
    const std::shared_ptr<Actor> actor = ghost->GetActor();

    const CellIndex currentCell = SelectNearestCell(GetGame().GetMap().FindCells(actor->GetRegion()), actor->GetDirection());;
    const CellIndex targetCell = ghost->SelectTargetCell();
    const MoveDirection backDirection = GetBackDirection(actor->GetDirection());

    // select turn
    const MoveDirection nextDirection = SelectBestDirection(currentCell, targetCell, backDirection);
    const CellIndex moveTarget = FindMoveTarget(currentCell, nextDirection);
    actor->MoveTo(nextDirection, moveTarget);
}

void AIController::FindWayOnScatterState()
{

}

void AIController::FindWayOnFrightenedState()
{

}

MoveDirection AIController::SelectBestDirection(const CellIndex& currentCell, const CellIndex& targetCell,
                                                const MoveDirection backDirection) const
{
    Map& map = GetGame().GetMap();

    const Position targetCellCenterPos = map.GetCellCenterPos(targetCell);
    const Math::Vector2f targetPos = Math::Vector2f(static_cast<float>(targetCellCenterPos.GetX()),
                                                    static_cast<float>(targetCellCenterPos.GetY()));

    const MapNeighborsInfo neighbors = map.GetDirectNeighbors(currentCell);
    MoveDirection result = MoveDirection::None;
    float minDistance = std::numeric_limits<float>::max();

    const auto findDistance = [this, &targetPos, &map](const CellIndex& startCell) -> float
    {
        const Position startCellCenterPos = map.GetCellCenterPos(startCell);
        const Math::Vector2f startPos = Math::Vector2f(static_cast<float>(startCellCenterPos.GetX()),
                                                       static_cast<float>(startCellCenterPos.GetY()));
        return (targetPos - startPos).Length();
    };

    for (const Neighbor& neighbor : neighbors.mNeighbors)
    {
        if (((neighbor.mCellType == MapCellType::Empty) || 
            ((neighbor.mCellType == MapCellType::Door) && (neighbor.mDirection == MoveDirection::Up))) && 
            (backDirection != neighbor.mDirection))
        {
            const CellIndex next = GetNext(currentCell, neighbor.mDirection);
            const float distance = findDistance(next);
            if (distance < minDistance)
            {
                result = neighbor.mDirection;
                minDistance = distance;
            }
        }
    }

    return result;
}

CellIndex AIController::FindMoveTarget(const CellIndex& currentCell, const MoveDirection direction)
{
    Map& map = GetGame().GetMap();
    CellIndex cellIndex = currentCell;
    size_t emptyNeighborsCount = 0;

    while (emptyNeighborsCount < 2)
    {
        emptyNeighborsCount = 0;
        const MapNeighborsInfo neighbors = map.GetDirectNeighbors(currentCell);

        for (const Neighbor& neighbor : neighbors.mNeighbors)
        {
            if ((neighbor.mCellType == MapCellType::Empty) ||
                ((neighbor.mDirection == MoveDirection::Up) && (neighbor.mCellType == MapCellType::Door))) // the door is passable from the bottom
                emptyNeighborsCount++;
        }

        const CellIndex next = GetNext(cellIndex, direction);
        if (map.GetCell(next) != MapCellType::Empty)
            break;
        cellIndex = next;
    }

    return cellIndex;
}

void AIController::SetupScheduler()
{
    const std::shared_ptr<Ghost> inky = mGhosts[GhostsFactory::kInky];
    const std::shared_ptr<Ghost> clyde = mGhosts[GhostsFactory::kClyde];

    auto inkyStart = [inky]() -> ActionResult
    {
        if (GetGame().GetDotsGrid().GetEatenDotsCount() >= 30)
        {
            inky->SetState(GhostState::Chase);
            return ActionResult::Unregister;
        }

        return ActionResult::None;
    };

    auto clydeStart = [clyde]() -> ActionResult
    {
        DotsGrid& dotsGrid = GetGame().GetDotsGrid();
        const size_t dotsEaten = dotsGrid.GetEatenDotsCount();
        const size_t dotsCount = dotsGrid.GetDotsCount();

        if ((static_cast<float>(dotsEaten) / static_cast<float>(dotsCount)) >= 0.33f)
        {
            clyde->SetState(GhostState::Chase);
            return ActionResult::Unregister;
        }

        return ActionResult::None;
    };

    Scheduler& scheduler = GetGame().GetScheduler();
    const std::shared_ptr<Action> inkyStartAction = std::make_shared<Action>(inkyStart);
    const std::shared_ptr<Action> clydeStartAction = std::make_shared<Action>(clydeStart);

    scheduler.RegisterAction(inkyStartAction, 0, true);
    scheduler.RegisterAction(clydeStartAction, 0, true);
}

} // Pacman namespace