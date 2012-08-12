#pragma once

#include "math/vector2.h"
#include "math/matrix4.h"
#include "drawable.h"

#include <memory>

namespace Pacman {

class StaticSceneNode
{
public:

	StaticSceneNode() = delete;
	StaticSceneNode(const std::shared_ptr<IDrawable> drawable);
	StaticSceneNode(const StaticSceneNode&) = default;
	virtual ~StaticSceneNode() {}

	StaticSceneNode& operator= (const StaticSceneNode&) = default;

	std::shared_ptr<IDrawable> GetDrawable() const
	{
		return mDrawable;
	}

private:

	std::shared_ptr<IDrawable> mDrawable;
};

class SceneNode : public StaticSceneNode
{
public:

	SceneNode() = delete;
	SceneNode(const std::shared_ptr<IDrawable> drawable, const Math::Vector2f position);
	SceneNode(const SceneNode&) = default;
	~SceneNode() = default;

	SceneNode& operator= (const SceneNode&) = default;

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
};

} // Pacman namespace
