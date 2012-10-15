#pragma once

#include "base.h"
#include "engine_forwdecl.h"
#include "color.h"
#include "math/matrix4.h"

namespace Pacman {

class Renderer
{
public:

	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer() = default;

	Renderer& operator= (const Renderer&) = delete;

	void Init(const size_t viewportWidth, const size_t viewportHeigth);

	void DrawFrame();

	void SetClearColor(const Color color)
	{
		mClearColor = color;
	}

	size_t GetViewportWidth() const
	{
		return mViewportWidth;
	}

	size_t GetViewportHeight() const
	{
		return mViewportHeight;
	}

private:

	void RenderDrawable(const IDrawable& drawable, const Math::Matrix4f modelMatrix);

	Math::Matrix4f mProjection;
	Color mClearColor;
	size_t mViewportWidth;
	size_t mViewportHeight;

    Texture2D* mLastTexture;
    ShaderProgram* mLastShaderProgram;
    bool mLastAlphaBlendState;
};

} // Pacman namespace
