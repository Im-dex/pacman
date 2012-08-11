#pragma once

#include "singleton.h"

#include <memory>
#include <cstdint>

namespace Pacman {

class AssetManager;
class FontManager;

static const size_t kScreenSizesCount = 4;
static const size_t kScreenDensitiesCount = 4;

enum class ScreenSize : uint8_t
{
	Small  = 0,
	Normal = 1,
	Large  = 2,
	XLarge = 3
};

enum class ScreenDensity : uint8_t
{
	Ldpi  = 0,
	Mpdi  = 1,
	Hdpi  = 2,
	Xhdpi = 3
};

class Engine : public Singleton<Engine>
{
public:

	Engine(const ScreenSize screenSize, const ScreenDensity screenDensity);
	Engine(const Engine&) = delete;
	~Engine() = default;

	Engine& operator= (const Engine&) = delete;

	void Init(const size_t screenWidth, const size_t screenHeight);

	void Deinit();

	ScreenSize GetScreenSize() const
	{
		return mScreenSize;
	}

	ScreenDensity GetScreenDensity() const
	{
		return mScreenDensity;
	}

	AssetManager& GetAssetManager()
	{
		return *mAssetManager;
	}

	FontManager& GetFontManager()
	{
		return *mFontManager;
	}

private:

	size_t mScreenWidth;
	size_t mScreenHeight;
	ScreenSize mScreenSize;
	ScreenDensity mScreenDensity;

	std::shared_ptr<AssetManager> mAssetManager;
	std::shared_ptr<FontManager>  mFontManager;
};

} // Pacman namespace