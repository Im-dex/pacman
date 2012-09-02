#pragma once

#include <memory>
#include <list>
#include <cstdint>

#include "map.h"

namespace Pacman {

class Map;
class Actor;
enum class ActorMoveDirection;

class ActorsManager
{
public:

    ActorsManager() = delete;
    ActorsManager(const std::shared_ptr<Map> map);
    ActorsManager(const ActorsManager&) = delete;
    ~ActorsManager() = default;

    ActorsManager& operator= (const ActorsManager&) = delete;

    void RegisterActor(std::shared_ptr<Actor> actor);

    void UnregisterActor(std::shared_ptr<Actor> actor);

    void Update(const uint64_t dt);

private:

    bool IsDirectionPossible(const ActorMoveDirection direction, const CellIndex& index) const;

    const std::shared_ptr<Map>        mMap;
    std::list<std::shared_ptr<Actor>> mActors;
};

} // Pacman namespace