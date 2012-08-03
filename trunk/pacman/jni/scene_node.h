#pragma once

#include "math/vector2.h"
#include "math/matrix4.h"
#include "drawable.h"
#include "unique_id.h"

namespace Pacman {

class SceneNode : public UniqueIdProvider
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

	const Math::Matrix4f GetModelMatrix() const
	{
		return mModelMatrix;
	}

	void Translate(const Math::Vector2f position);

private:

	Math::Matrix4f mModelMatrix;
	Drawable	   mDrawable;
};

} // Pacman namespace
