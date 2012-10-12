#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <cassert>

#include "base.h"
#include "game_typedefs.h"
#include "actor_controller.h"

namespace Pacman {

class IDrawable;
class Actor;
class Ghost;
class GameLoader;
class Scheduler;
class SpriteSheet;
class Map;
class DotsGrid;
class PacmanController;

static const size_t kGhostsCount = 4;

struct DirectionDiscard
{
    CellIndex mCell;
    MoveDirection mDirection;
};

struct AIInfo
{
    std::array<CellIndex, kGhostsCount> mScatterTargets;
    uint64_t                            mScatterDuration;
    uint64_t                            mScatterInterval;
    std::vector<DirectionDiscard>       mDiscardCells;
    uint64_t                            mFrightDuration;
};

class AIController : public IActorController
{
public:

    AIController(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr);
    AIController(const AIController&) = delete;
    ~AIController() = default;

    AIController& operator= (const AIController&) = delete;

    void Update(const uint64_t dt);

    std::shared_ptr<Actor> GetActor(const GhostId ghostId) const;

    CellIndex GetScatterTarget(const GhostId ghostid) const;

    void EnableFrightenedState();

    void DisableFrightenedState();

    virtual void OnDirectionChanged(const MoveDirection newDirection);

    virtual void OnTargetAchieved();

private:

    enum class SelectDirectionMethod
    {
        Best,
        Random
    };

    enum class SelectTargetMethod
    {
        OwnBehavior,
        Scatter
    };

    typedef std::array<std::shared_ptr<Ghost>, kGhostsCount> GhostsArray; 

    void FindWayOnWaitState();

    void FindWayOnLeaveHouse();

    void FindWayOnChaseState();

    void FindWayOnScatterState();

    void FindWayOnFrightenedState();

    void FindWay(const SelectDirectionMethod directionMethod, const SelectTargetMethod targetMethod);

    MoveDirection SelectBestDirection(const CellIndex& currentCell, const CellIndex& targetCell,
                                      const MoveDirection backDirection) const;

    MoveDirection SelectRandomDirection(const CellIndex& currentCell, const MoveDirection backDirection) const;

    CellIndex FindMoveTarget(const CellIndex& currentCell, const MoveDirection direction);

    void SetupScheduler();

    const AIInfo               mAIInfo;
    GhostsArray                mGhosts;
    size_t                     mCurrentGhost;
    std::shared_ptr<IDrawable> mFrightenedDrawable;
};

} // Pacman namespace