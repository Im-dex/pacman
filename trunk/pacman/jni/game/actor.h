#pragma once

#include <memory>

#include "game_forwdecl.h"
#include "engine_typedefs.h"
#include "game_typedefs.h"

namespace Pacman {

class Actor
{
public:

    Actor() = delete;
    Actor(const Size size, const Speed speed, const Position& startPosition,
          const MoveDirection startDirection, const std::shared_ptr<IDrawable>& startDrawable);

    Actor(const Actor&) = delete;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = delete;

    void AttachToScene(SceneManager& sceneManager) const;

    void DetachFromScene(SceneManager& sceneManager) const;

    void Update(const uint64_t dt, IActorController& controller);

    Position GetCenterPos() const;

    SpriteRegion GetRegion() const;

    void Rotate(const Rotation& rotation);

    void TranslateToCell(const CellIndex& cell);

    void TranslateToPosition(const Position& position);

    void Move(const MoveDirection direction, const Size wayLength);

    void MoveTo(const MoveDirection direction, const CellIndex& cell);

    CellIndex FindMaxAvailableCell(const MoveDirection direction) const;

    void SetDrawable(const std::shared_ptr<IDrawable>& drawable);

    MoveDirection GetDirection() const
    {
        return mDirection;
    }

    Position GetStartPosition() const
    {
        return mStartPosition;
    }

    MoveDirection GetStartDirection() const
    {
        return mStartDirection;
    }

private:

    void DoMove(IActorController& controller, const PosOffset offset,
                const bool recursive);

    const Size                            mSize;
    const Speed                           mSpeed;
    const Position                        mPivotOffset;
    const Position                        mStartPosition;
    const MoveDirection                   mStartDirection;
    Position                              mMoveTarget;
    MoveDirection                         mDirection;
    std::shared_ptr<SceneNode>            mNode;
    bool                                  mDirectionChanged;
};

} // Pacman namespace