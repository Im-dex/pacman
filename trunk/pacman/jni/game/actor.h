#pragma once

#include "math/vector2.h"

#include <memory>

namespace Pacman {

class SceneNode;

enum class ActorMoveDirection
{
    None,
    Left,
    Right,
    Up,
    Down
};

class Actor
{
public:

    Actor() = delete;
    Actor(std::shared_ptr<SceneNode> node, const Math::Vector2s& startCellIndices,
          const ActorMoveDirection startDirection, const size_t moveSpeed);
    Actor(const Actor&) = default;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = default;

    void Translate(const Math::Vector2f& position);

    void Move(const Math::Vector2f& offset);

    Math::Vector2s GetMapCellIndices() const
    {
        return mMapCellIndices;
    }

    ActorMoveDirection GetDirection() const
    {
        return mDirection;
    }

    ActorMoveDirection GetNextDirection() const
    {
        return mNextDirection;
    }

    size_t GetSpeed() const
    {
        return mSpeed;
    }

    void SetMapCellIndices(const Math::Vector2s& cellIndices)
    {
        mMapCellIndices = cellIndices;
    }

    void SetDirection(const ActorMoveDirection direction)
    {
        mDirection = direction;
    }

    void SetNextDirection(const ActorMoveDirection direction)
    {
        mNextDirection = direction;
    }

private:

    Math::Vector2s             mMapCellIndices;
    ActorMoveDirection         mDirection;
    ActorMoveDirection         mNextDirection;
    size_t                     mSpeed;
    std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace