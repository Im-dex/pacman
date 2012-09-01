#include "actors_manager.h"
#include "map.h"
#include "actor.h"

#include <cstdint>

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
        CellIndex index = actorPtr->GetMapCellIndex();

        ActorMoveDirection nextDirection = actorPtr->GetNextDirection();
        if ((nextDirection != ActorMoveDirection::None) && IsDirectionPossible(nextDirection, index))
        {
            actorPtr->SetDirection(nextDirection);
            actorPtr->SetNextDirection(ActorMoveDirection::None);
        }

        ActorMoveDirection direction = actorPtr->GetDirection();
        if ((direction == ActorMoveDirection::None) || !IsDirectionPossible(direction, index))
            continue;

        size_t speed = actorPtr->GetSpeed();
        actorPtr->Move(CalcOffset(direction, speed, dt));
    }
}

bool ActorsManager::IsDirectionPossible(const ActorMoveDirection direction, const CellIndex& index) const
{
    MapNeighborsInfo neighbors = mMap->GetDirectNeighbors(index);

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