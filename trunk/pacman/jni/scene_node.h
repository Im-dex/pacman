#pragma once

#include <memory>

#include "base.h"
#include "engine_typedefs.h"
#include "math/matrix4.h"
#include "drawable.h"

namespace Pacman {

class SceneNode
{
public:

	SceneNode() = delete;
	SceneNode(const std::shared_ptr<IDrawable> drawable, const SpritePosition& pivotOffset,
              const SpritePosition& position, const Rotation& rotation);
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

	void Move(const int32_t xOffset, const int32_t yOffset)
	{
        mPosition.SetX(static_cast<uint32_t>(static_cast<const uint16_t>(mPosition.GetX() + xOffset)));
        mPosition.SetY(static_cast<uint32_t>(static_cast<const uint16_t>(mPosition.GetY() + yOffset)));
	}

	void Translate(const SpritePosition& position)
	{
		mPosition = position;
	}

    void Rotate(const Rotation& rotation)
    {
        mRotation = rotation;
    }

private:
	
    SpritePosition             mPivotOffset;
	SpritePosition             mPosition;
    Rotation                   mRotation;
    std::shared_ptr<IDrawable> mDrawable;
};

} // Pacman namespace
