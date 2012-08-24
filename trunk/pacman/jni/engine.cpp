#include "engine.h"
#include "error.h"
#include "asset_manager.h"
#include "font_manager.h"
#include "scene_manager.h"
#include "renderer.h"
#include "timer.h"
#include "json/json.h"

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
	Json::Value root;
	Json::Reader reader;

	std::string configData = mAssetManager->LoadTextFile("config.json");
	bool result = reader.parse(configData, root, false);
	PACMAN_CHECK_ERROR(result, ErrorCode::BadFormat);

	Json::Value resoution = root["base_resolution"];
	mBaseWidth = resoution["width"].asUInt();
	mBaseHeight = resoution["height"].asUInt();
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