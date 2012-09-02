#pragma once

#include <memory>

#include "map.h"

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
    Actor(const std::string& textData, std::shared_ptr<SceneNode> node);
    Actor(std::shared_ptr<SceneNode> node, const CellIndex& startIndex,
          const ActorMoveDirection startDirection, const size_t moveSpeed);
    Actor(const Actor&) = default;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = default;

    void Translate(const Math::Vector2f& position);

    void Move(const Math::Vector2f& offset);

    CellIndex GetMapCellIndex() const
    {
        return mMapCellIndex;
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

    void SetMapCellIndex(const CellIndex& index)
    {
        mMapCellIndex = index;
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

    CellIndex                  mMapCellIndex;
    ActorMoveDirection         mDirection;
    ActorMoveDirection         mNextDirection;
    size_t                     mSpeed;
    std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace