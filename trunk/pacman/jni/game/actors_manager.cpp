#include "actors_manager.h"
#include "map.h"
#include "actor.h"

namespace Pacman {

static Math::Vector2f CalcOffset(const ActorMoveDirection direction, const size_t speed, const uint64_t dt)
{
    const size_t offset = speed * dt;

    switch (direction)
    {
    case ActorMoveDirection::Left:
        return Math::Vector2f(-offset, 0);
    case ActorMoveDirection::Right:
        return Math::Vector2f(offset, 0);
    case ActorMoveDirection::Up:
        return Math::Vector2f(0, -offset);
    case ActorMoveDirection::Down:
        return Math::Vector2f(0, offset);
    default:
        return Math::Vector2f(0, 0);
    }
}

ActorsManager::ActorsManager(std::shared_ptr<Map> map)
             : mMap(map),
               mActors()
{
}

void ActorsManager::RegisterActor(std::shared_ptr<Actor> actor)
{
    mActors.push_back(actor);
}

void ActorsManager::UnregisterActor(std::shared_ptr<Actor> actor)
{
    mActors.remove(actor);
}

void ActorsManager::Update(const uint64_t dt)
{
    for (auto actorPtr : mActors)
    {
        Math::Vector2s cellIndices = actorPtr->GetMapCellIndices();

        ActorMoveDirection nextDirection = actorPtr->GetNextDirection();
        if ((nextDirection != ActorMoveDirection::None) && IsDirectionPossible(nextDirection, cellIndices))
        {
            actorPtr->SetDirection(nextDirection);
            actorPtr->SetNextDirection(ActorMoveDirection::None);
        }

        ActorMoveDirection direction = actorPtr->GetDirection();
        if ((direction == ActorMoveDirection::None) || !IsDirectionPossible(direction, cellIndices))
            continue;

        size_t speed = actorPtr->GetSpeed();
        actorPtr->Move(CalcOffset(direction, speed, dt));
    }
}

bool ActorsManager::IsDirectionPossible(const ActorMoveDirection direction, const Math::Vector2s& cellIndices) const
{
    MapNeighborsInfo neighbors = mMap->GetDirectNeighbors(cellIndices);

    switch (direction)
    {
    case ActorMoveDirection::Left:
        return neighbors.left == MapCellType::Empty;
    case ActorMoveDirection::Right:
        return neighbors.right == MapCellType::Empty;
    case ActorMoveDirection::Up:
        return neighbors.top == MapCellType::Empty;
    case ActorMoveDirection::Down:
        return neighbors.bottom == MapCellType::Empty;
    default: // none
        return true;
    }
}

} // Pacman namespace