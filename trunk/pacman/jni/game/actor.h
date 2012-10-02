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

class Actor
{
public:

    static const CellIndex::value_t kMax;

    Actor() = delete;
    Actor(const Size size, const Speed speed, const Size cellSize,
          const Position& startPosition, const std::shared_ptr<IDrawable>& startDrawable,
          const std::shared_ptr<Map>& map, const std::shared_ptr<IActorListener>& listener);

    Actor(const Actor&) = delete;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = delete;

    void AttachToScene(SceneManager& sceneManager) const;

    void DetachFromScene(SceneManager& sceneManager) const;

    void Update(const uint64_t dt);

    Position GetCenterPos() const;

    SpriteRegion GetRegion() const;

    void Rotate(const Rotation& rotation);

    void TranslateTo(const CellIndex& cell);

    // if cellsCount == kMax then actor will move to the max available cell
    void Move(const MoveDirection direction, const CellIndex::value_t cellsCount);

    void SetDrawable(const std::shared_ptr<IDrawable>& drawable);

    MoveDirection GetDirection() const
    {
        return mDirection;
    }

private:

    CellIndex FindMaxAvailableCell(const CellIndex& currentCellIndex, const MoveDirection direction) const;

    const Size                            mSize;
    const Speed                           mSpeed;
    const Size                            mCellSize;
    const Position                        mPivotOffset;
    const std::shared_ptr<Map>            mMap;
    const std::shared_ptr<IActorListener> mListener;
    Position                              mMoveTarget;
    MoveDirection                         mDirection;
    std::shared_ptr<SceneNode>            mNode;
};

} // Pacman namespace