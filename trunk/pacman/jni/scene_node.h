#pragma once

#include "math/vector2.h"
#include "math/matrix4.h"
#include "drawable.h"

namespace Pacman {

class SceneNode
{
public:

	SceneNode() = delete;
	SceneNode(const Drawable& drawable, const Math::Vector2f position);
	SceneNode(const SceneNode&) = default;
	~SceneNode() = default;

	SceneNode& operator= (const SceneNode&) = default;

	Drawable GetDrawable()
	{
		return mDrawable;
	}

	Math::Matrix4f GetModelMatrix() const;

	void Translate(const Math::Vector2f position)
	{
		mPosition = position;
	}

	Math::Vector2f GetPosition() const
	{
		return mPosition;
	}

	void Move(const Math::Vector2f offset)
	{
		mPosition += offset;
	}

private:

	Math::Vector2f mPosition;
	Drawable	   mDrawable;
};

} // Pacman namespace
