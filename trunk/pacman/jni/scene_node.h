#pragma once

#include "math/vector2.h"
#include "math/matrix4.h"
#include "drawable.h"

#include <cstdint>

namespace Pacman {

class SceneNode
{
public:

	SceneNode() = delete;
	SceneNode(const Drawable& drawable, const Math::Vector2f position);
	SceneNode(const SceneNode&) = default;
	~SceneNode() = default;

	SceneNode& operator= (const SceneNode&) = default;

	uint64_t GetId() const
	{
		return mId;
	}

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
	uint64_t	   mId;

	static uint64_t gIdCounter;
};

} // Pacman namespace
