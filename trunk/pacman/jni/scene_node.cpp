#include "scene_node.h"

#include "base.h"

namespace Pacman {

SceneNode::SceneNode(const std::shared_ptr<IDrawable> drawable, const SpritePosition& position)
	: mDrawable(drawable),
	  mPosition(position)
      //mRotation(0.0f)
{
}

Math::Matrix4f SceneNode::GetModelMatrix() const
{
    const float x = static_cast<float>(mPosition.GetX());
    const float y = static_cast<float>(mPosition.GetY());
	return Math::Matrix4f(Math::Matrix4f::kIdentity).Translate(x, y, 0.0f);
}

} // Pacman namespace
