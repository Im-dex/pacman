#include "actor.h"

namespace Pacman {

Actor::Actor(std::shared_ptr<SceneNode> node, const Math::Vector2s& startCell,
             const ActorMoveDirection startDirection, const size_t moveSpeed)
     : mMapCell(startCell),
       mDirection(ActorMoveDirection::None),
       mSpeed(moveSpeed),
       mNode(node)
{
}

} // Pacman namespace