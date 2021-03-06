#pragma once

#include <memory>

#include "base.h"
#include "engine_forwdecl.h"
#include "engine_listeners.h"

namespace Pacman {

class Engine
{
public:

	Engine();
	Engine(const Engine&) = delete;
	~Engine();

	Engine& operator= (const Engine&) = delete;

	void Start(const size_t screenWidth, const size_t screenHeight);

	void OnDrawFrame();

	void OnTouch(const int event, const float x, const float y);

    void ShowMessage(const std::string& message) const;

    void ShowInfo(const std::string& message, const std::string& title, const bool terminate);

	void SetListener(const std::shared_ptr<IEngineListener> listener)
    {
        mListener = std::move(listener);
    }

	AssetManager& GetAssetManager() const
	{
		return *mAssetManager;
	}

	SceneManager& GetSceneManager() const
	{
		return *mSceneManager;
	}

	Renderer& GetRenderer() const
	{
		return *mRenderer;
	}

    InputManager& GetInputManager() const
    {
        return *mInputManager;
    }

    bool IsStarted() const
    {
        return mStarted;
    }

private:

	std::unique_ptr<AssetManager> mAssetManager;
	std::unique_ptr<SceneManager> mSceneManager;
	std::unique_ptr<Renderer>	  mRenderer;
    std::unique_ptr<InputManager> mInputManager;
	std::unique_ptr<Timer>		  mTimer;
	
	std::shared_ptr<IEngineListener> mListener;
	uint64_t						 mLastTime;

	size_t mBaseWidth;
	size_t mBaseHeight;
    bool   mStarted;
};

Engine& GetEngine();

} // Pacman namespace