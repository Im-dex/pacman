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

static FORCEINLINE std::shared_ptr<IDrawable> GetDirectionDrawable(const Ghost& ghost)
{
    switch (ghost.GetActor().GetDirection())
    {
    case MoveDirection::Left:
        return ghost.GetLeftDrawable();
    case MoveDirection::Right:
        return ghost.GetRightDrawable();
    case MoveDirection::Up:
        return ghost.GetTopDrawable();
    case MoveDirection::Down:
        return ghost.GetBottomDrawable();
    }
}

AIController::AIController(const Size actorSize, const SpriteSheet& spriteSheet)
            : mAIInfo(GetGame().GetLoader().LoadAIInfo("ai.json")),
              mCurrentGhost(kGhostsCount)
{
    GhostsFactory factory;
    mGhosts[EnumCast(GhostId::Blinky)] = factory.CreateGhost(actorSize, spriteSheet, GhostId::Blinky);
    mGhosts[EnumCast(GhostId::Pinky)] = factory.CreateGhost(actorSize, spriteSheet, GhostId::Pinky);
    mGhosts[EnumCast(GhostId::Inky)] = factory.CreateGhost(actorSize, spriteSheet, GhostId::Inky);
    mGhosts[EnumCast(GhostId::Clyde)] = factory.CreateGhost(actorSize, spriteSheet, GhostId::Clyde);

    ResetState();
    SetupScheduler();

    mFrightenedDrawable = spriteSheet.MakeSprite("enemy_frightened", SpriteRegion(0, 0, actorSize, actorSize));
}

void AIController::Update(const uint64_t dt)
{
    for (size_t i = 0; i < kGhostsCount; i++)
    {
        mCurrentGhost = i;
        GetCurrentGhost().GetActor().Update(dt, *this);
    }
}

Ghost& AIController::GetGhost(const GhostId ghostId) const
{
    return *(mGhosts[EnumCast(ghostId)]);
}

Actor& AIController::GetGhostActor(const GhostId ghostId) const
{
    return GetGhost(ghostId).GetActor();
}

CellIndex AIController::GetScatterTarget(const GhostId ghostid) const
{
    return mAIInfo.mScatterTargets[EnumCast(ghostid)];
}

void AIController::OnDirectionChanged(const MoveDirection newDirection)
{
    Ghost& ghost = GetCurrentGhost();
    if (ghost.GetState() == GhostState::Frightened)
        return;

    ghost.GetActor().SetDrawable(GetDirectionDrawable(ghost));
}

void AIController::OnTargetAchieved()
{
    switch (GetCurrentGhost().GetState())
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
        PACMAN_CHECK_ERROR(false);
    }
}

void AIController::EnableFrightenedState()
{
    for (const std::unique_ptr<Ghost>& ghost : mGhosts)
    {
        Actor& actor = ghost->GetActor();

        if ((ghost->GetState() == GhostState::Wait) ||
            (ghost->GetState() == GhostState::LeaveHouse))
        {
            continue;
        }

        ghost->SetState(GhostState::Frightened);
        actor.SetDrawable(mFrightenedDrawable);

        const MoveDirection backDirection = GetBackDirection(actor.GetDirection());
        const CellIndex currentCell = SelectNearestCell(GetGame().GetMap().FindCells(actor.GetRegion()), backDirection);
        const CellIndex newTarget = FindMoveTarget(currentCell, backDirection);
        actor.MoveTo(backDirection, newTarget);
    }

    const auto restoreAction = []() -> ActionResult
    {
        GetGame().GetAIController().DisableFrightenedState();
        return ActionResult::Unregister;
    };

    GetGame().GetScheduler().RegisterEvent(restoreAction, mAIInfo.mFrightDuration, false);
}

void AIController::DisableFrightenedState()
{
    for (const std::unique_ptr<Ghost>& ghost : mGhosts)
    {
        if (ghost->GetState() == GhostState::Frightened)
        {
            ghost->SetState(GhostState::Chase);
            ghost->GetActor().SetDrawable(GetDirectionDrawable(*ghost));
        }
    }
}

void AIController::ResetState()
{
    for (const std::unique_ptr<Ghost>& ghost : mGhosts)
    {
        Actor& actor = ghost->GetActor();
        ghost->SetState(ghost->GetStartState());
        actor.TranslateToPosition(actor.GetStartPosition());
        const CellIndex startTargetCell = actor.FindMaxAvailableCell(actor.GetStartDirection());
        actor.MoveTo(actor.GetStartDirection(), startTargetCell);
    }

    SetupInkyClydeStartActions();
}

void AIController::OnGhostDead(const GhostId ghostId)
{
    Ghost& ghost = GetGhost(ghostId);
    Actor& actor = ghost.GetActor();
    ghost.SetState(GhostState::LeaveHouse);
    actor.TranslateToPosition(mAIInfo.mRespawn);
    const CellIndex startTargetCell = actor.FindMaxAvailableCell(MoveDirection::Up);
    actor.MoveTo(MoveDirection::Up, startTargetCell);
}

GhostId AIController::GetCurrentGhostId() const
{
    return MakeEnum<GhostId>(static_cast<EnumType<GhostId>::value>(mCurrentGhost));
}

Ghost& AIController::GetCurrentGhost() const
{
    return GetGhost(GetCurrentGhostId());
}

// move up & down
void AIController::FindWayOnWaitState()
{
    Map& map = GetGame().GetMap();
    Actor& actor = GetCurrentGhost().GetActor();
    const MoveDirection direction = actor.GetDirection();

    const CellIndex currentCell = SelectNearestCell(map.FindCells(actor.GetRegion()), direction);
    const CellIndex next = GetNext(currentCell, direction);
    if (map.GetCell(next) != MapCellType::Empty)
    {
        const MoveDirection backDirection = GetBackDirection(direction);
        const CellIndex newTargetCell = actor.FindMaxAvailableCell(backDirection);
        actor.MoveTo(backDirection, newTargetCell);
    }
}

void AIController::FindWayOnLeaveHouse()
{
    Map& map = GetGame().GetMap();
    Ghost& ghost = GetCurrentGhost();
    Actor& actor = ghost.GetActor();

    const Size mapCenterX = map.GetPosition().GetX() + ((map.GetColumnsCount() * map.GetCellSize()) / 2);
    const Position actorCenterPos = actor.GetCenterPos();
    if (actorCenterPos.GetX() != mapCenterX)
    {
        if (actorCenterPos.GetX() > mapCenterX)
            actor.Move(MoveDirection::Left, actorCenterPos.GetX() - mapCenterX);
        else
            actor.Move(MoveDirection::Right, mapCenterX - actorCenterPos.GetX());
    }
    else
    {
        const CellIndex targetCell = actor.FindMaxAvailableCell(MoveDirection::Up);
        actor.MoveTo(MoveDirection::Up, targetCell);
        ghost.SetState(GhostState::Chase);
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
    Ghost& ghost = GetCurrentGhost();
    Actor& actor = ghost.GetActor();

    const CellIndex currentCell = SelectNearestCell(GetGame().GetMap().FindCells(actor.GetRegion()), actor.GetDirection());
    const CellIndex targetCell = (targetMethod == SelectTargetMethod::OwnBehavior) ? ghost.SelectTargetCell()
                                                                                   : GetScatterTarget(GetCurrentGhostId());
    const MoveDirection backDirection = GetBackDirection(actor.GetDirection());

    // select turn
    const MoveDirection nextDirection = (directionMethod == SelectDirectionMethod::Best) ? SelectBestDirection(currentCell, targetCell, backDirection)
                                                                                         : SelectRandomDirection(currentCell, backDirection);
    const CellIndex moveTarget = FindMoveTarget(currentCell, nextDirection);
    actor.MoveTo(nextDirection, moveTarget);
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

    const auto iter = std::find_if(mAIInfo.mDiscardCells.begin(), mAIInfo.mDiscardCells.end(), 
                                   [&currentCell](const DirectionDiscard& directionDiscard) -> bool
    {
        return directionDiscard.mCell == currentCell;
    });

    const MoveDirection discardedDirection = (iter == mAIInfo.mDiscardCells.end()) ? MoveDirection::None
                                                                                   : iter->mDirection;

    for (const Neighbor& neighbor : neighbors.mNeighbors)
    {
        if (((neighbor.mCellType == MapCellType::Empty) || 
            ((neighbor.mCellType == MapCellType::Door) && (neighbor.mDirection == MoveDirection::Up))) && 
            (backDirection != neighbor.mDirection) &&
            ((discardedDirection == MoveDirection::None) || (discardedDirection != neighbor.mDirection)))
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

    PACMAN_CHECK_ERROR(result != MoveDirection::None);
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

void AIController::SetupInkyClydeStartActions()
{
    const auto inkyStartAction = []() -> ActionResult
    {
        Game& game = GetGame();
        if (game.GetDotsGrid().GetEatenDotsCount() >= 30)
        {
            game.GetAIController().GetGhost(GhostId::Inky).SetState(GhostState::LeaveHouse);
            return ActionResult::Unregister;
        }

        return ActionResult::None;
    };

    const auto clydeStartAction = []() -> ActionResult
    {
        Game& game = GetGame();
        DotsGrid& dotsGrid = game.GetDotsGrid();
        const size_t dotsEaten = dotsGrid.GetEatenDotsCount();
        const size_t dotsCount = dotsGrid.GetDotsCount();

        if ((static_cast<float>(dotsEaten) / static_cast<float>(dotsCount)) >= 0.33f)
        {
            game.GetAIController().GetGhost(GhostId::Clyde).SetState(GhostState::LeaveHouse);
            return ActionResult::Unregister;
        }

        return ActionResult::None;
    };

    Scheduler& scheduler = GetGame().GetScheduler();
    scheduler.RegisterTrigger(inkyStartAction);
    scheduler.RegisterTrigger(clydeStartAction);
}

void AIController::SetupScheduler()
{
    typedef EnumType<GhostId>::value GhostIdValueT;

    const auto disableScatterAction = []() -> ActionResult
    {
        AIController& aiController = GetGame().GetAIController();
        for (GhostIdValueT i = 0; i < kGhostsCount; i++)
        {
            Ghost& ghost = aiController.GetGhost(MakeEnum<GhostId>(i));
            if (ghost.GetState() == GhostState::Scatter)
                ghost.SetState(GhostState::Chase);
        }
        return ActionResult::None;
    };

    const uint64_t scatterDuration = mAIInfo.mScatterDuration;
    const auto enableScatterAction = [disableScatterAction, scatterDuration]() -> ActionResult
    {
        AIController& aiController = GetGame().GetAIController();
        for (GhostIdValueT i = 0; i < kGhostsCount; i++)
        {
            Ghost& ghost = aiController.GetGhost(MakeEnum<GhostId>(i));
            if (ghost.GetState() == GhostState::Chase)
                ghost.SetState(GhostState::Scatter);
        }

        GetGame().GetScheduler().RegisterEvent(disableScatterAction, scatterDuration, false);
        return ActionResult::None;
    };

    Scheduler& scheduler = GetGame().GetScheduler();
    scheduler.RegisterEvent(enableScatterAction, mAIInfo.mScatterInterval, true);

    Map& map = GetGame().GetMap();
    const CellIndex leftTunnelExit = map.GetLeftTunnelExit();
    const CellIndex rightTunnelExit = map.GetRightTunnelExit();

    for (GhostIdValueT i = 0; i < kGhostsCount; i++)
    {
        const GhostId ghostId = MakeEnum<GhostId>(i);

        const auto leftTunnelAction = [ghostId, leftTunnelExit, rightTunnelExit]() -> ActionResult
        {
            const CellIndexArray ghostCells = GetGame().GetSharedDataManager().GetGhostCells(ghostId);
            if (ghostCells.size() == 1)
            {
                Actor& actor = GetGame().GetAIController().GetGhostActor(ghostId);
                if ((ghostCells[0] == leftTunnelExit) && (actor.GetDirection() == MoveDirection::Left))
                {
                    actor.TranslateToCell(rightTunnelExit);
                }
            }
            return ActionResult::None;
        };

        const auto rightTunnelAction = [ghostId, leftTunnelExit, rightTunnelExit]() -> ActionResult
        {
            const CellIndexArray ghostCells = GetGame().GetSharedDataManager().GetGhostCells(ghostId);
            if (ghostCells.size() == 1)
            {
                Actor& actor = GetGame().GetAIController().GetGhostActor(ghostId);
                if ((ghostCells[0] == rightTunnelExit) && (actor.GetDirection() == MoveDirection::Right))
                {
                    actor.TranslateToCell(leftTunnelExit);
                }
            }
            return ActionResult::None;
        };

        scheduler.RegisterTrigger(leftTunnelAction);
        scheduler.RegisterTrigger(rightTunnelAction);
    }
}

} // Pacman namespace