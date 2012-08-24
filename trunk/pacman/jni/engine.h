#pragma once

#include <memory>
#include <cstdint>
#include <cassert>

namespace Pacman {

class AssetManager;
class FontManager;
class SceneManager;
class Renderer;
class Game;
class Timer;

class IEngineListener
{
public:

	virtual void OnLoad() = 0;

	virtual void OnUnload() = 0;

	virtual void OnUpdate(const uint64_t dt) = 0;
};

class Engine
{
public:

	Engine();
	Engine(const Engine&) = delete;
	~Engine();

	Engine& operator= (const Engine&) = delete;

	void Init(const size_t screenWidth, const size_t screenHeight);

	void OnDrawFrame();

	void OnTouch();

	void Deinit();

	void SetListener(std::shared_ptr<IEngineListener> listener)
	{
		mListener = listener;
	}

	AssetManager& GetAssetManager()
	{
		assert(mAssetManager != nullptr);
		return *mAssetManager;
	}

	FontManager& GetFontManager()
	{
		assert(mFontManager != nullptr);
		return *mFontManager;
	}

	SceneManager& GetSceneManager()
	{
		assert(mSceneManager != nullptr);
		return *mSceneManager;
	}

	Renderer& GetRenderer()
	{
		assert(mRenderer != nullptr);
		return *mRenderer;
	}

private:

	std::unique_ptr<AssetManager> mAssetManager;
	std::unique_ptr<FontManager>  mFontManager;
	std::unique_ptr<SceneManager> mSceneManager;
	std::unique_ptr<Renderer>	  mRenderer;
	std::unique_ptr<Timer>		  mTimer;
	
	std::shared_ptr<IEngineListener> mListener;
	uint64_t						 mLastTime;

	size_t mBaseWidth;
	size_t mBaseHeight;
};

Engine* GetEngine();

} // Pacman namespace