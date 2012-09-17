#pragma once

#include <memory>

#include "engine_typedefs.h"
#include "game_typedefs.h"
#include "game_listeners.h"

namespace Pacman {

class SceneNode;
class IDrawable;
class SceneManager;
class Map;

class Actor : public std::enable_shared_from_this<Actor>
{
public:

    static const uint16_t kMax;

    Actor() = delete;
    Actor(const uint16_t size, const uint16_t speed, const uint16_t cellSize,
          const SpritePosition& startPosition, const std::shared_ptr<IDrawable> startDrawable,
          const std::shared_ptr<Map> map, const std::shared_ptr<IActorListener> listener);

    Actor(const Actor&) = delete;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = delete;

    void AttachToScene(SceneManager& sceneManager) const;

    void DetachFromScene(SceneManager& sceneManager) const;

    void Update(const uint64_t dt);

    SpritePosition GetCenterPos() const;

    SpriteRegion GetRegion() const;

    void Rotate(const Rotation& rotation);

    void Translate(const CellIndex& cell);

    // if cellsCount == kMax then actor will move to the max available cell
    void Move(const MoveDirection direction, const uint16_t cellsCount);

    void SetDrawable(const std::shared_ptr<IDrawable> drawable);

    MoveDirection GetDirection() const
    {
        return mDirection;
    }

private:

    CellIndex FindMaxAvailableCell(const CellIndex& curCell, const MoveDirection direction) const;

    const uint16_t                        mSize;
    const uint16_t                        mSpeed;
    const uint16_t                        mCellSize;
    const SpritePosition                  mCenterOffset;
    const std::shared_ptr<Map>            mMap;
    const std::shared_ptr<IActorListener> mListener;
    SpritePosition                        mMoveTarget;
    MoveDirection                         mDirection;
    std::shared_ptr<SceneNode>            mNode;
};

} // Pacman namespace