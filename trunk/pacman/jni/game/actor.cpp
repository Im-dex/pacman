#include "actor.h"

#include "error.h"
#include "scene_node.h"
#include "scene_manager.h"
#include "json_helper.h"
#include "map.h"

namespace Pacman {

Actor::Actor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
             const uint16_t cellSize, std::shared_ptr<IDrawable> drawable)
     : mSize(size),
       mSpeed(speed),
       mMovePoint(0, 0),
       mCellSize(cellSize),
       mNode(std::make_shared<SceneNode>(drawable, SpritePosition(size/2, size/2), startPosition, Rotation::kZero))
{
}

void Actor::AttachToScene(SceneManager& sceneManager) const
{
    sceneManager.AttachNode(mNode);
}

void Actor::DetachFromScene(SceneManager& sceneManager) const
{
    sceneManager.DetachNode(mNode);
}

void Actor::Update(const uint64_t dt)
{
    const int32_t offset = static_cast<const int32_t>(mSpeed * mCellSize * dt / 1000);

    const SpritePosition currentPosition = mNode->GetPosition();
    const int32_t xDiff = static_cast<const int32_t>(mMovePoint.GetX()) - currentPosition.GetX();
    const int32_t yDiff = static_cast<const int32_t>(mMovePoint.GetY()) - currentPosition.GetY();

    const int32_t xOffset = ((xDiff < 0) ? -1 : 1) * (std::min(offset, std::abs(xDiff)));
    const int32_t yOffset = ((yDiff < 0) ? -1 : 1) * (std::min(offset, std::abs(yDiff)));

    mNode->Move(xOffset, yOffset);
}

SpritePosition Actor::GetPosition() const
{
    return mNode->GetPosition();
}

SpriteRegion Actor::GetRegion() const
{
    return SpriteRegion(mNode->GetPosition(), mSize, mSize);
}

void Actor::Rotate(const Rotation& rotation)
{
    mNode->Rotate(rotation);
}

} // Pacman namespace