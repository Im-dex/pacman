#include "scene_node.h"

namespace Pacman {

SceneNode::SceneNode(const std::shared_ptr<IDrawable>& drawable,
                     const Position& position, const Rotation& rotation)
	: mDrawable(drawable),
      mPivotOffset(Position::kZero),
	  mPosition(position),
      mRotation(rotation),
      mModelMatrix(Math::Matrix4f::kIdentity),
      mChanged(false)
{
}

Math::Matrix4f SceneNode::GetModelMatrix()
{
    if (mChanged)
    {
        const float posX = static_cast<float>(mPosition.GetX());
        const float posY = static_cast<float>(mPosition.GetY());

        const float pivotOffsetX = static_cast<float>(mPivotOffset.GetX());
        const float pivotOffsetY = static_cast<float>(mPivotOffset.GetY());

        mModelMatrix = Math::Matrix4f::MakeTranslation(posX + pivotOffsetX, posY + pivotOffsetY, 0.0f) * 
                                                       Math::Matrix4f::MakeRotationZ(mRotation.GetZ()) *
                                                       Math::Matrix4f::MakeTranslation(-pivotOffsetX, -pivotOffsetY, 0.0f); 

        mChanged = false;
    }
    
    return mModelMatrix;
}

} // Pacman namespace
