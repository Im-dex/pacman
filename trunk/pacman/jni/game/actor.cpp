#include "actor.h"
#include "scene_node.h"

namespace Pacman {

Actor::Actor(std::shared_ptr<SceneNode> node, const Math::Vector2s& startCellIndices,
             const ActorMoveDirection startDirection, const size_t moveSpeed)
     : mMapCellIndices(startCellIndices),
       mDirection(startDirection),
       mNextDirection(ActorMoveDirection::None),
       mSpeed(moveSpeed),
       mNode(node)
{
}

void Actor::Translate(const Math::Vector2f& position)
{
    mNode->Translate(position);
}

void Actor::Move(const Math::Vector2f& offset)
{
    mNode->Move(offset);
}

} // Pacman namespace