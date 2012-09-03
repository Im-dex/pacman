#pragma once

#include <memory>

#include "engine_typedefs.h"
#include "math/matrix4.h"
#include "drawable.h"

namespace Pacman {

class SceneNode
{
public:

	SceneNode() = delete;
	SceneNode(const std::shared_ptr<IDrawable> drawable, const SpritePosition& position);
	SceneNode(const SceneNode&) = default;
	~SceneNode() = default;

	SceneNode& operator= (const SceneNode&) = default;

	Math::Matrix4f GetModelMatrix() const;

    std::shared_ptr<IDrawable> GetDrawable() const
    {
        return mDrawable;
    }

	SpritePosition GetPosition() const
	{
		return mPosition;
	}

	void MoveForward(const SpritePosition& offset)
	{
		mPosition += offset;
	}

    void MoveBack(const SpritePosition& offset)
    {
        mPosition -= offset;
    }

	void Translate(const SpritePosition& position)
	{
		mPosition = position;
	}

    /*void Rotate(const float angle)
    {

    }*/

private:
	
	SpritePosition             mPosition;
    //float                      mRotation;
    std::shared_ptr<IDrawable> mDrawable;
};

} // Pacman namespace
