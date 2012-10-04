#include "ai_controller.h"

#include "actor.h"
#include "ghost.h"
#include "loader.h"
#include "spritesheet.h"
#include "map.h"
#include "ghosts_factory.h"

namespace Pacman {

AIController::AIController(const GameLoader& loader, const Size actorSize, const std::shared_ptr<Map>& map,
                           const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
{
    GhostsFactory factory;
    mGhosts[0] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kBlinky);
    mGhosts[1] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kPinky);
    mGhosts[2] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kInky);
    mGhosts[3] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, GhostsFactory::kClyde);
}

void AIController::Update(const uint64_t dt)
{
    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        ghost->GetActor()->Update(dt, this);
    }
}

std::shared_ptr<Actor> AIController::GetActor(const size_t index) const
{
    return mGhosts[index]->GetActor();
}

void AIController::OnDirectionChanged(const MoveDirection newDirection)
{
    
}

void AIController::OnTargetAchieved()
{
    
}

} // Pacman namespace