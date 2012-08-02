#include "scene_node.h"

namespace Pacman {

uint64_t SceneNode::gIdCounter = 0;

SceneNode::SceneNode(const Drawable& drawable, const Math::Vector2f position)
		 : mDrawable(drawable),
		   mModelMatrix(),
		   mId(gIdCounter++)
{
	Translate(position);
}

void SceneNode::Translate(const Math::Vector2f position)
{
	mModelMatrix.Translate(position.GetX(), position.GetY(), 0.0f);
}

} // Pacman namespace
