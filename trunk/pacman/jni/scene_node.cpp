#include "scene_node.h"

#include "base.h"

namespace Pacman {

SceneNode::SceneNode(const Drawable& drawable, const Math::Vector2f position)
		 : mDrawable(drawable),
		   mPosition(position)
{
}

Math::Matrix4f SceneNode::GetModelMatrix() const
{
	//LOGI("Pos: %f, %f", x, y);
	return Math::Matrix4f(Math::Matrix4f::kIdentity).Translate(mPosition.GetX(), mPosition.GetY(), 0.0f);
}

} // Pacman namespace
