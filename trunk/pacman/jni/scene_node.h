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
	SceneNode(const std::shared_ptr<IDrawable>& drawable,
              const Position& position, const Rotation& rotation);

	SceneNode(const SceneNode&) = default;
	~SceneNode() = default;

	SceneNode& operator= (const SceneNode&) = default;

	Math::Matrix4f GetModelMatrix();

    std::shared_ptr<IDrawable> GetDrawable() const
    {
        return mDrawable;
    }

    void SetDrawable(const std::shared_ptr<IDrawable>& drawable)
    {
        mDrawable = drawable;
    }

	Position GetPosition() const
	{
		return mPosition;
	}

	void Move(const PosOffset xOffset, const PosOffset yOffset)
	{
        mPosition.SetX(static_cast<const Position::value_t>(mPosition.GetX() + xOffset));
        mPosition.SetY(static_cast<const Position::value_t>(mPosition.GetY() + yOffset));
        mChanged = true;
	}

	void Translate(const Position& position)
	{
		mPosition = position;
        mChanged = true;
	}

    void SetRotation(const Rotation& rotation, const Position& pivotOffset)
    {
        mRotation = rotation;
        mPivotOffset = pivotOffset;
        mChanged = true;
    }

private:
	
    Position                   mPivotOffset;
	Position                   mPosition;
    Rotation                   mRotation;
    Math::Matrix4f             mModelMatrix;
    bool                       mChanged;
    std::shared_ptr<IDrawable> mDrawable;
};

} // Pacman namespace
