#include "ai_controller.h"

#include "actor.h"
#include "ghost.h"
#include "loader.h"
#include "spritesheet.h"
#include "map.h"
#include "ghosts_factory.h"

namespace Pacman {

class ActorsListener : public IActorListener
{
public:

    ActorsListener() = default;
    ActorsListener(const ActorsListener&) = delete;
    ~ActorsListener() = default;

    ActorsListener& operator= (const ActorsListener&) = delete;

    virtual void OnDirectionChanged(Actor& actor, const MoveDirection newDirection) 
    {
        
    }

    virtual void OnTargetAchieved(Actor& actor) 
    {
        
    }
};

AIController::AIController(const GameLoader& loader, const Size actorSize, const std::shared_ptr<Map>& map,
                           const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
{
    GhostsFactory factory;
    const std::shared_ptr<ActorsListener> listener = std::make_shared<ActorsListener>();
    mGhosts[0] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, listener, GhostsFactory::kBlinky);
    mGhosts[1] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, listener, GhostsFactory::kPinky);
    mGhosts[2] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, listener, GhostsFactory::kInky);
    mGhosts[3] = factory.CreateGhost(loader, actorSize, map, spriteSheetPtr, listener, GhostsFactory::kClyde);
}

void AIController::Update(const uint64_t dt)
{
    for (const std::shared_ptr<Ghost>& ghost : mGhosts)
    {
        ghost->GetActor()->Update(dt);
    }
}

std::shared_ptr<Actor> AIController::GetActor(const size_t index) const
{
    return mGhosts[index]->GetActor();
}

} // Pacman namespace