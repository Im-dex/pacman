#include "engine.h"
#include "error.h"
#include "asset_manager.h"
#include "font_manager.h"
#include "scene_manager.h"
#include "renderer.h"
#include "input_manager.h"
#include "timer.h"
#include "json_helper.h"

#include <unistd.h>

namespace Pacman {

Engine::Engine()
	  : mAssetManager(new AssetManager()),
	    mFontManager(new FontManager()),
		mSceneManager(new SceneManager()),
		mRenderer(new Renderer()),
        mInputManager(new InputManager()),
		mTimer(new Timer()),
		mListener(nullptr),
		mLastTime(0)
{
	std::string configData = mAssetManager->LoadTextFile("config.json");
	const Json::Value root = JsonHelper::ParseJson(configData);
	PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);
	
	const Json::Value resoution = root["base_resolution"];
	PACMAN_CHECK_ERROR(resoution.isObject(), ErrorCode::BadFormat);
	
	const Json::Value width = resoution["width"];
	const Json::Value height = resoution["height"];
	PACMAN_CHECK_ERROR(width.isNumeric() && height.isNumeric(), ErrorCode::BadFormat);

	mBaseWidth = width.asUInt();
	mBaseHeight = height.asUInt();
}

Engine::~Engine()
{
}

void Engine::Init(const size_t screenWidth, const size_t screenHeight)
{
	size_t resolutionMultiplier = std::min(screenWidth / mBaseWidth, screenHeight / mBaseHeight);
	mAssetManager->SetMultiplier(resolutionMultiplier);
	mRenderer->Init(screenWidth, screenHeight);
	if (mListener != nullptr)
		mListener->OnLoad();

	mTimer->Start();
	mLastTime = mTimer->GetMillisec();
}

static const size_t kFramesPerSecond = 25;
static const size_t kSkipTicks = 1000 / kFramesPerSecond;

void Engine::OnDrawFrame()
{
    mInputManager->Update();
	if (mListener != nullptr)
		mListener->OnUpdate(kSkipTicks);
	mRenderer->DrawFrame();

	mLastTime += kSkipTicks;
	size_t sleepTime = mLastTime - mTimer->GetMillisec();
	if (sleepTime >= 0)
		usleep(sleepTime * 1000);
}

void Engine::OnTouch(const int event, const float x, const float y)
{
    PACMAN_CHECK_ERROR((event >= 0) && (event < kTouchEventsCount), ErrorCode::BadArgument);

    const uint8_t u8event = static_cast<const uint8_t>(event);
    const TouchInfo info = { static_cast<TouchEvent>(u8event), x, y };
	mInputManager->PushInfo(info);
}

void Engine::Deinit()
{
	
}

} // Pacman namespace