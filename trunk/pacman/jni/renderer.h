#pragma once

#include "color.h"
#include "math/matrix4.h"

#include <memory>

namespace Pacman {

class SceneManager;
class Drawable;

class Renderer
{
public:

	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer() = default;

	Renderer& operator= (const Renderer&) = delete;

	void Init(const size_t viewportWidth, const size_t viewportHeigth);

	void DrawFrame();

	void SetClearColor(const Color& color)
	{
		mClearColor = color;
	}

	void SetSceneManager(const std::shared_ptr<SceneManager> sceneManager)
	{
		mSceneManager = sceneManager;
	}

private:

	void RenderDrawable(const Drawable& drawable, const Math::Matrix4f modelMatrix);

	Math::Matrix4f mProjection;
	Color mClearColor;
	std::shared_ptr<SceneManager> mSceneManager;
};

} // Pacman namespace
