#include "engine.h"
#include "asset_manager.h"
#include "font_manager.h"

namespace Pacman {

Engine::Engine(const ScreenSize screenSize, const ScreenDensity screenDensity)
	  : mScreenWidth(0),
	    mScreenHeight(0),
		mScreenSize(screenSize),
		mScreenDensity(screenDensity),
		mAssetManager(nullptr),
		mFontManager(nullptr)
{}

Engine::~Engine()
{
}

void Engine::Init(const size_t screenWidth, const size_t screenHeight)
{
	mAssetManager = std::unique_ptr<AssetManager>(new AssetManager());
	mFontManager = std::unique_ptr<FontManager>(new FontManager());
}

void Engine::Deinit()
{
	mAssetManager = nullptr;
}

} // Pacman namespace