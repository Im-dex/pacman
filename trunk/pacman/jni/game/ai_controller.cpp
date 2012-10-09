#include "ai_controller.h"

#include <cstdlib>

#include "log.h"
#include "common.h"
#include "utils.h"
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
#include "shared_data_manager.h"
#include "scheduler.h"
#include "dots_grid.h"

namespace Pacman {

static const CellIndex::value_t kWayLength = 1;

AIController::AIController(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
            : mAIInfo(GetGame().GetLoader().LoadAIInfo("ai.json")),
              mCurrentGhost(kGhostsCount)
{
    GhostsFactory factory;
    mGhosts[EnumCast(GhostId::Blinky)] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostId::Blinky);
    mGhosts[EnumCast(GhostId::Pinky)] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostId::Pinky);
    mGhosts[EnumCast(GhostId::Inky)] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostId::Inky);
    mGhosts[EnumCast(GhostId::Clyde)] = factory.CreateGhost(actorSize, spriteSheetPtr, GhostId::Clyde);

    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        const std::shared_ptr<Actor> actor = ghost->GetActor();
        const CellIndex startTargetCell = actor->FindMaxAvailableCell(actor->GetDirection());
        actor->MoveTo(actor->GetDirection(), startTargetCell);
    }

    SetupScheduler();

    const std::shared_ptr<SpriteSheet> spriteSheet = spriteSheetPtr.lock();
    PACMAN_CHECK_ERROR(spriteSheet != nullptr, ErrorCode::BadArgument);
    mFrightenedDrawable = spriteSheet->MakeSprite("enemy_frightened", SpriteRegion(0, 0, actorSize, actorSize));
}

void AIController::Update(const uint64_t dt)
{
    for (size_t i = 0; i < kGhostsCount; i++)
    {
        mCurrentGhost = i;
        mGhosts[i]->GetActor()->Update(dt, this);
    }
}

std::shared_ptr<Actor> AIController::GetActor(const GhostId ghostId) const
{
    return mGhosts[EnumCast(ghostId)]->GetActor();
}

CellIndex AIController::GetScatterTarget(const GhostId ghostid) const
{
    return mAIInfo.mScatterTargets[EnumCast(ghostid)];
}

void AIController::OnDirectionChanged(const MoveDirection newDirection)
{
    const std::shared_ptr<Ghost> ghost = mGhosts[mCurrentGhost];
    if (ghost->GetState() == GhostState::Frightened)
        return;

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
    case GhostState::LeaveHouse:
        FindWayOnLeaveHouse();
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

void AIController::EnableFrightenedState()
{
    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        const std::shared_ptr<Actor> actor = ghost->GetActor();

        if ((ghost->GetState() == GhostState::Wait) ||
            (ghost->GetState() == GhostState::LeaveHouse))
        {
            continue;
        }

        ghost->SetState(GhostState::Frightened);
        actor->SetDrawable(mFrightenedDrawable);

        const MoveDirection backDirection = GetBackDirection(actor->GetDirection());
        const CellIndex currentCell = SelectNearestCell(GetGame().GetMap().FindCells(actor->GetRegion()), backDirection);
        const CellIndex newTarget = FindMoveTarget(currentCell, backDirection);
        actor->MoveTo(backDirection, newTarget);
    }

    const auto restore = []() -> ActionResult
    {
        GetGame().GetAIController().DisableFrightenedState();
        return ActionResult::Unregister;
    };

    const std::shared_ptr<Action> restoreAction = std::make_shared<Action>(restore);
    GetGame().GetScheduler().RegisterAction(restoreAction, mAIInfo.mFrightDuration, false);
}

void AIController::DisableFrightenedState()
{
    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        if (ghost->GetState() == GhostState::Frightened)
            ghost->SetState(GhostState::Chase);
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

void AIController::FindWayOnLeaveHouse()
{
    Map& map = GetGame().GetMap();
    const std::shared_ptr<Ghost> ghost = mGhosts[mCurrentGhost];
    const std::shared_ptr<Actor> actor = ghost->GetActor();

    const Size mapCenterX = map.GetPosition().GetX() + ((map.GetColumnsCount() * map.GetCellSize()) / 2);
    const Position actorCenterPos = actor->GetCenterPos();
    if (actorCenterPos.GetX() != mapCenterX)
    {
        if (actorCenterPos.GetX() > mapCenterX)
            actor->Move(MoveDirection::Left, actorCenterPos.GetX() - mapCenterX);
        else
            actor->Move(MoveDirection::Right, mapCenterX - actorCenterPos.GetX());
    }
    else
    {
        const CellIndex targetCell = actor->FindMaxAvailableCell(MoveDirection::Up);
        actor->MoveTo(MoveDirection::Up, targetCell);
        ghost->SetState(GhostState::Chase);
    }
}

// move to the nearest crossroad
void AIController::FindWayOnChaseState()
{
    FindWay(SelectDirectionMethod::Best, SelectTargetMethod::OwnBehavior);
}

void AIController::FindWayOnScatterState()
{
    FindWay(SelectDirectionMethod::Best, SelectTargetMethod::Scatter);
}

void AIController::FindWayOnFrightenedState()
{
    FindWay(SelectDirectionMethod::Random, SelectTargetMethod::OwnBehavior);
}

void AIController::FindWay(const SelectDirectionMethod directionMethod, const SelectTargetMethod targetMethod)
{
    const std::shared_ptr<Ghost> ghost = mGhosts[mCurrentGhost];
    const std::shared_ptr<Actor> actor = ghost->GetActor();

    const CellIndex currentCell = SelectNearestCell(GetGame().GetMap().FindCells(actor->GetRegion()), actor->GetDirection());
    const CellIndex targetCell = (targetMethod == SelectTargetMethod::OwnBehavior) ? ghost->SelectTargetCell()
                                                                                   : GetScatterTarget(MakeEnum<GhostId>(static_cast<EnumType<GhostId>::value>(mCurrentGhost)));
    const MoveDirection backDirection = GetBackDirection(actor->GetDirection());

    // select turn
    const MoveDirection nextDirection = (directionMethod == SelectDirectionMethod::Best) ? SelectBestDirection(currentCell, targetCell, backDirection)
                                                                                         : SelectRandomDirection(currentCell, backDirection);
    const CellIndex moveTarget = FindMoveTarget(currentCell, nextDirection);
    actor->MoveTo(nextDirection, moveTarget);
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

MoveDirection AIController::SelectRandomDirection(const CellIndex& currentCell, const MoveDirection backDirection) const
{
    const MapNeighborsInfo neighbors = GetGame().GetMap().GetDirectNeighbors(currentCell);
    std::vector<MoveDirection> possibleDirections;

    for (const Neighbor& neighbor : neighbors.mNeighbors)
    {
        if ((neighbor.mCellType == MapCellType::Empty) &&
            (neighbor.mDirection != backDirection))
        {
            possibleDirections.push_back(neighbor.mDirection);
        }
    }

    srand(time(nullptr));
    const size_t randVal = static_cast<size_t>(rand() % possibleDirections.size());
    return possibleDirections[randVal];
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
    const std::shared_ptr<Ghost> inky = mGhosts[EnumCast(GhostId::Inky)];
    const std::shared_ptr<Ghost> clyde = mGhosts[EnumCast(GhostId::Clyde)];

    auto inkyStart = [inky]() -> ActionResult
    {
        if (GetGame().GetDotsGrid().GetEatenDotsCount() >= 30)
        {
            inky->SetState(GhostState::LeaveHouse);
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
            clyde->SetState(GhostState::LeaveHouse);
            return ActionResult::Unregister;
        }

        return ActionResult::None;
    };

    Scheduler& scheduler = GetGame().GetScheduler();
    const std::shared_ptr<Action> inkyStartAction = std::make_shared<Action>(inkyStart);
    const std::shared_ptr<Action> clydeStartAction = std::make_shared<Action>(clydeStart);

    scheduler.RegisterAction(inkyStartAction, 0, true);
    scheduler.RegisterAction(clydeStartAction, 0, true);

    Map& map = GetGame().GetMap();
    const CellIndex leftTunnelExit = map.GetLeftTunnelExit();
    const CellIndex rightTunnelExit = map.GetRightTunnelExit();

    for (EnumType<GhostId>::value i = 0; i < kGhostsCount; i++)
    {
        const GhostId ghostId = MakeEnum<GhostId>(i);

        auto leftTunnel = [ghostId, leftTunnelExit, rightTunnelExit]() -> ActionResult
        {
            const CellIndexArray ghostCells = GetGame().GetSharedDataManager().GetGhostCells(ghostId);
            if (ghostCells.size() == 1)
            {
                const std::shared_ptr<Actor> actor = GetGame().GetAIController().GetActor(ghostId);
                if ((ghostCells[0] == leftTunnelExit) && (actor->GetDirection() == MoveDirection::Left))
                    actor->TranslateToCell(rightTunnelExit);
            }
            return ActionResult::None;
        };

        auto rightTunnel = [ghostId, leftTunnelExit, rightTunnelExit]() -> ActionResult
        {
            const CellIndexArray ghostCells = GetGame().GetSharedDataManager().GetGhostCells(ghostId);
            if (ghostCells.size() == 1)
            {
                const std::shared_ptr<Actor> actor = GetGame().GetAIController().GetActor(ghostId);
                if ((ghostCells[0] == rightTunnelExit) && (actor->GetDirection() == MoveDirection::Right))
                    actor->TranslateToCell(leftTunnelExit);
            }
            return ActionResult::None;
        };

        Scheduler& scheduler = GetGame().GetScheduler();
        const std::shared_ptr<Action> leftTunnelAction = std::make_shared<Action>(leftTunnel);
        const std::shared_ptr<Action> rightTunnelAction = std::make_shared<Action>(rightTunnel);
        scheduler.RegisterAction(leftTunnelAction, 0, true);
        scheduler.RegisterAction(rightTunnelAction, 0, true);
    }
}

} // Pacman namespace