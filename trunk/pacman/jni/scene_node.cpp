#include "scene_node.h"

namespace Pacman {

SceneNode::SceneNode(const std::shared_ptr<IDrawable> drawable, const SpritePosition& pivotOffset,
                     const SpritePosition& position, const Rotation& rotation)
	: mDrawable(drawable),
      mPivotOffset(pivotOffset),
	  mPosition(position),
      mRotation(rotation)
{
}

Math::Matrix4f SceneNode::GetModelMatrix() const
{
    const float posX = static_cast<float>(mPosition.GetX());
    const float posY = static_cast<float>(mPosition.GetY());

    const float pivotOffsetX = static_cast<float>(mPivotOffset.GetX());
    const float pivotOffsetY = static_cast<float>(mPivotOffset.GetY());

	return Math::Matrix4f::MakeTranslation(posX + pivotOffsetX, posY + pivotOffsetY, 0.0f) * 
           Math::Matrix4f::MakeRotationZ(mRotation.GetZ()) *
           Math::Matrix4f::MakeTranslation(-pivotOffsetX, -pivotOffsetY, 0.0f);
}

} // Pacman namespace
