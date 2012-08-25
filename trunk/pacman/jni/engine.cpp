#include "engine.h"
#include "error.h"
#include "asset_manager.h"
#include "font_manager.h"
#include "scene_manager.h"
#include "renderer.h"
#include "timer.h"
#include "json_helper.h"

namespace Pacman {

Engine::Engine()
	  : mAssetManager(new AssetManager()),
	    mFontManager(new FontManager()),
		mSceneManager(new SceneManager()),
		mRenderer(new Renderer()),
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

void Engine::OnDrawFrame()
{
	const uint64_t curTime = mTimer->GetMillisec();
	const uint64_t dt = curTime - mLastTime;
	mLastTime = curTime;

	if (mListener != nullptr)
		mListener->OnUpdate(dt);
	mRenderer->DrawFrame();

	//mLastTime = mTimer->GetMillisec();
}

void Engine::OnTouch()
{
	LOGI("TOUCH!");
}

void Engine::Deinit()
{
	
}

} // Pacman namespace