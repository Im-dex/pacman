#include "engine.h"

#include <unistd.h>

#include "log.h"
#include "main.h"
#include "error.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "renderer.h"
#include "input_manager.h"
#include "timer.h"
#include "jni_utility.h"
#include "json_helper.h"

namespace Pacman {

static const size_t kFramesPerSecond = 25;
static const size_t kSkipTicks = 1000 / kFramesPerSecond;

static FORCEINLINE void showLoadingDialog()
{
    JNI::CallStaticVoidMethod("com/imdex/pacman/NativeLib", "showLoadingDialog", "()V");
}

static FORCEINLINE void hideLoadingDialog()
{
    JNI::CallStaticVoidMethod("com/imdex/pacman/NativeLib", "hideLoadingDialog", "()V");
}

Engine::Engine()
	  : mAssetManager(new AssetManager()),
		mSceneManager(new SceneManager()),
		mRenderer(new Renderer()),
        mInputManager(new InputManager()),
		mTimer(new Timer()),
		mListener(nullptr),
		mLastTime(0),
        mBaseWidth(0),
        mBaseHeight(0),
        mStarted(false)
{
}

Engine::~Engine()
{
}

void Engine::Start(const size_t screenWidth, const size_t screenHeight)
{
    if (!mStarted)
    {
        LogI("Engine start");
        std::string configData = mAssetManager->LoadTextFile("config.json");
        const JsonHelper::Value root(configData);

        const JsonHelper::Value resolution = root.GetValue<JsonHelper::Value>("base_resolution");
        mBaseWidth = resolution.GetValue<size_t>("width");
        mBaseHeight = resolution.GetValue<size_t>("height");
    }
    else
    {
        mListener->OnStop(*this);
        mListener = nullptr;
        mAssetManager = nullptr;
        mSceneManager = nullptr;
        mRenderer = nullptr;
        mInputManager = nullptr;
        mTimer = nullptr;
        mLastTime = 0;
        ErrorHandler::CleanGLErrors();
        PACMAN_CHECK_GL_ERROR();
    }
    
    mAssetManager = std::unique_ptr<AssetManager>(new AssetManager());
    mSceneManager = std::unique_ptr<SceneManager>(new SceneManager());
    mRenderer = std::unique_ptr<Renderer>(new Renderer());
    mInputManager = std::unique_ptr<InputManager>(new InputManager());
    mTimer = std::unique_ptr<Timer>(new Timer());
    PacmanSetEngineListener(*this);
    PACMAN_CHECK_ERROR(mListener != nullptr, ErrorCode::InvalidState);

	const size_t resolutionMultiplier = std::min(screenWidth / mBaseWidth, screenHeight / mBaseHeight);
	mAssetManager->SetMultiplier(resolutionMultiplier);
	mRenderer->Init(screenWidth, screenHeight);
    mStarted = true;

    showLoadingDialog();
    mListener->OnStart(*this);
    hideLoadingDialog();

	mTimer->Start();
	mLastTime = mTimer->GetMillisec();
}

void Engine::OnDrawFrame()
{
    mInputManager->Update();
	if (mListener != nullptr)
		mListener->OnUpdate(*this, kSkipTicks);
	mRenderer->DrawFrame();

	mLastTime += kSkipTicks;
    const uint64_t now = mTimer->GetMillisec();
	if (mLastTime >= now)
    {
        const uint64_t sleepTime = mLastTime - now;
		usleep(sleepTime * 1000);
    }
}

void Engine::OnTouch(const int event, const float x, const float y)
{
    typedef EnumType<TouchEvent>::value TouchEventValueT;
    const TouchEventValueT eventValue = static_cast<TouchEventValueT>(event);
    const TouchInfo info = { MakeEnum<TouchEvent>(eventValue), x, y };
	mInputManager->PushInfo(info);
}

} // Pacman namespace