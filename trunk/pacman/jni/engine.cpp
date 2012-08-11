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

void Engine::Init(const size_t screenWidth, const size_t screenHeight)
{
	mAssetManager = std::make_shared<AssetManager>();
	mFontManager = std::make_shared<FontManager>();
}

void Engine::Deinit()
{
	mAssetManager = nullptr;
}

} // Pacman namespace