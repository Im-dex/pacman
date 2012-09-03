#include "actors_manager.h"

#include <cstdint>
#include <algorithm>

#include "error.h"
#include "engine.h"
#include "scene_manager.h"
#include "map.h"
#include "actor.h"

namespace Pacman {

static const uint16_t kNpos = uint16_t(-1);

static uint16_t CalcMaxAvailableDistance(const ActorMoveDirection direction, const SpritePosition& curPosition,
                                         const SpritePosition& maxAvailablePos)
{
    switch (direction)
    {
    case ActorMoveDirection::Left:
        return curPosition.GetX() - maxAvailablePos.GetX();
    case ActorMoveDirection::Right:
        return maxAvailablePos.GetX() - curPosition.GetX();
    case ActorMoveDirection::Up:
        return curPosition.GetY() - maxAvailablePos.GetY();
    case ActorMoveDirection::Down:
        return maxAvailablePos.GetY() - curPosition.GetY();
    default:
        return kNpos;
    }
}

ActorsManager::ActorsManager(const std::shared_ptr<Map> map)
             : mMap(map),
               mActors()
{
}

void ActorsManager::RegisterActor(std::shared_ptr<Actor> actor)
{
    mActors.push_back(ActorData { actor, kNpos } );
    actor->AttachToScene(GetEngine()->GetSceneManager());
}

void ActorsManager::UnregisterActor(std::shared_ptr<Actor> actor)
{
    std::remove_if(mActors.begin(), mActors.end(), [&actor](const ActorData& data) -> bool
    {
        return data.mActor == actor;
    });

    actor->DetachFromScene(GetEngine()->GetSceneManager());
}

void ActorsManager::Update(const uint64_t dt)
{
    for (ActorData& actorData : mActors)
    {
        ActorMoveDirection nextDirection = actorData.mActor->GetNextDirection();
        if (nextDirection != ActorMoveDirection::None)
        {
            LOGI("direction: %u", nextDirection);
            const SpriteRegion curRegion = actorData.mActor->GetRegion();
            LOGI("Cur region: centerX: %u, centerY: %u, x: %u, y: %u, width: %u, height: %u", curRegion.GetCenterPosX(), curRegion.GetCenterPosY(), curRegion.GetPosX(), curRegion.GetPosY(), curRegion.GetWidth(), curRegion.GetHeight());
            const CellIndex index = mMap->FindCell(curRegion);
            LOGI("Cell index: row: %u, column: %u", index.GetX(), index.GetY());

            actorData.mActor->SetDirection(nextDirection);
            actorData.mActor->SetNextDirection(ActorMoveDirection::None);

            const SpritePosition maxAvailablePos = FindMaxAvailablePos(index, nextDirection);
            LOGI("max avail pos: x: %u, y: %u", maxAvailablePos.GetX(), maxAvailablePos.GetY());
            actorData.mAvailableDistance = CalcMaxAvailableDistance(nextDirection, curRegion.GetCenterPos(), maxAvailablePos);
            LOGI("max avail distance: %u", actorData.mAvailableDistance);
        }

        PACMAN_CHECK_ERROR(actorData.mAvailableDistance != kNpos, ErrorCode::InvalidState);

        const uint16_t speed = actorData.mActor->GetSpeed();
        const uint16_t cellSize = mMap->GetCellSize();
        LOGI("speed: %u, cellSize: %u", speed, cellSize);
        const uint16_t offset = std::min(actorData.mAvailableDistance, static_cast<const uint16_t>(speed * cellSize * dt / 1000));
        LOGI("offset: %u, mAvail: %u", offset, actorData.mAvailableDistance);

        if (offset > 0)
        {
            switch (actorData.mActor->GetDirection())
            {
            case ActorMoveDirection::Left:
                actorData.mActor->MoveBack(SpritePosition(offset, 0));
                break;
            case ActorMoveDirection::Right:
                actorData.mActor->MoveForward(SpritePosition(offset, 0));
                break;
            case ActorMoveDirection::Up:
                actorData.mActor->MoveBack(SpritePosition(0, offset));
                break;
            case ActorMoveDirection::Down:
                actorData.mActor->MoveForward(SpritePosition(0, offset));
                break;
            }

            actorData.mAvailableDistance -= offset;
        }
        else
        {
            // TODO: callback??
        }
    }
}

SpritePosition ActorsManager::FindMaxAvailablePos(const CellIndex& curCellIndex, const ActorMoveDirection direction)
{
    CellIndex index = curCellIndex;
    bool canMove = true;

    while (canMove)
    {
        MapNeighborsInfo neighbors = mMap->GetDirectNeighbors(index);

        switch (direction)
        {
        case ActorMoveDirection::Left:
            {
                if (neighbors.left == MapCellType::Empty)
                    index.SetY(index.GetY() - 1);
                else
                    canMove = false;
                break;
            }
        case ActorMoveDirection::Right:
            {
                if (neighbors.right == MapCellType::Empty)
                    index.SetY(index.GetY() + 1);
                else
                    canMove = false;
                break;
            }
        case ActorMoveDirection::Up:
            {
                if (neighbors.top == MapCellType::Empty)
                    index.SetX(index.GetX() - 1);
                else
                    canMove = false;
                break;
            }
        case ActorMoveDirection::Down:
            {
                if (neighbors.bottom == MapCellType::Empty)
                    index.SetX(index.GetX() + 1);
                else
                    canMove = false;
                break;
            }
        default: // none
            canMove = false;
        }
    }

    return mMap->GetCellCenterPos(index);
}

} // Pacman namespace