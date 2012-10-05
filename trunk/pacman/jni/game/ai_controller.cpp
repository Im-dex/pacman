#include "ai_controller.h"

#include "actor.h"
#include "ghost.h"
#include "loader.h"
#include "spritesheet.h"
#include "map.h"
#include "ghosts_factory.h"

namespace Pacman {

static const CellIndex::value_t kWayLength = 1;

AIController::AIController(const GameLoader& loader, const Size actorSize, const std::shared_ptr<Map>& map,
                           const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
            : mAIInfo(loader.LoadAIInfo("ai.json")),
              mCurrentGhost(nullptr)
{
    GhostsFactory factory;
    mGhosts[0] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kBlinky);
    mGhosts[1] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kPinky);
    mGhosts[2] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kInky);
    mGhosts[3] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kClyde);

    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        const std::shared_ptr<Actor>& actor = ghost->GetActor();
        actor->Move(actor->GetDirection(), kWayLength, false);
    }
}

void AIController::Update(const uint64_t dt)
{
    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        mCurrentGhost = ghost.get();
        ghost->GetActor()->Update(dt, this);
    }

    mCurrentGhost = nullptr;
}

std::shared_ptr<Actor> AIController::GetActor(const size_t index) const
{
    return mGhosts[index]->GetActor();
}

void AIController::OnDirectionChanged(const MoveDirection newDirection)
{
    switch (newDirection)
    {
    case MoveDirection::Left:
        mCurrentGhost->GetActor()->SetDrawable(mCurrentGhost->GetLeftDrawable());
        break;
    case MoveDirection::Right:
        mCurrentGhost->GetActor()->SetDrawable(mCurrentGhost->GetRightDrawable());
        break;
    case MoveDirection::Up:
        mCurrentGhost->GetActor()->SetDrawable(mCurrentGhost->GetTopDrawable());
        break;
    case MoveDirection::Down:
        mCurrentGhost->GetActor()->SetDrawable(mCurrentGhost->GetBottomDrawable());
        break;
    default:
        break;
    }
}

void AIController::OnTargetAchieved()
{
    
}

} // Pacman namespace