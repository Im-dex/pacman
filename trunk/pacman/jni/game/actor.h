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
    Actor(std::shared_ptr<SceneNode> node, const Math::Vector2s& startCell,
          const ActorMoveDirection startDirection, const size_t moveSpeed);
    Actor(const Actor&) = default;
    ~Actor() = default;

    Actor& operator= (const Actor&) = default;

private:

    Math::Vector2s             mMapCell;
    ActorMoveDirection         mDirection;
    size_t                     mSpeed;
    std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace