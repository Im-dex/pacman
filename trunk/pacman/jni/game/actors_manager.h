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

    SpritePosition FindMaxAvailablePos(const CellIndex& curCellIndex, const ActorMoveDirection direction);

    struct ActorData
    {
        std::shared_ptr<Actor> mActor;
        uint16_t               mAvailableDistance;
    };

    const std::shared_ptr<Map> mMap;
    std::list<ActorData>       mActors;
};

} // Pacman namespace