#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <cassert>

#include "base.h"
#include "game_typedefs.h"
#include "actor_controller.h"
#include "utils.h"

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
    Position                            mRespawn;
};

class AIController : public IActorController
{
public:

    AIController(const Size actorSize, const SpriteSheet& spriteSheet);
    AIController(const AIController&) = delete;
    ~AIController() = default;

    AIController& operator= (const AIController&) = delete;

    void Update(const uint64_t dt);

    Ghost& GetGhost(const GhostId ghostId) const;

    Actor& GetGhostActor(const GhostId ghostId) const;

    CellIndex GetScatterTarget(const GhostId ghostid) const;

    void EnableFrightenedState();

    void DisableFrightenedState();

    void ResetState();

    void OnGhostDead(const GhostId ghostId);

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

    typedef std::array<std::unique_ptr<Ghost>, kGhostsCount> GhostsArray; 

    GhostId GetCurrentGhostId() const;

    Ghost& GetCurrentGhost() const;

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

    void SetupInkyClydeStartActions();

    void SetupScheduler();

    const AIInfo               mAIInfo;
    GhostsArray                mGhosts;
    size_t                     mCurrentGhost;
    std::shared_ptr<IDrawable> mFrightenedDrawable;
};

} // Pacman namespace