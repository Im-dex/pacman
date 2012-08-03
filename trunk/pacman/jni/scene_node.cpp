#include "scene_node.h"

namespace Pacman {

SceneNode::SceneNode(const Drawable& drawable, const Math::Vector2f position)
		 : UniqueIdProvider(),
		   mDrawable(drawable),
		   mModelMatrix(Math::Matrix4f::kIdentity)
{
	Translate(position);
	//mModelMatrix.RotateZ();
	//mModelMatrix.Rotate(0.0f, 0.0f, 360.0f * (M_PI / 180));
}

void SceneNode::Translate(const Math::Vector2f position)
{
	mModelMatrix.Translate(position.GetX(), position.GetY(), 0.0f);
}

} // Pacman namespace
