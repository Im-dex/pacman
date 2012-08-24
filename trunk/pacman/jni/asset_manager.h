#pragma once

#include "base.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Pacman {

class Texture2D;
enum class TextureFiltering;
enum class TextureRepeat;

class AssetManager
{
public:

	AssetManager() = default;
	AssetManager(const AssetManager&) = delete;
	~AssetManager() = default;

	AssetManager& operator= (const AssetManager&) = delete;

	std::shared_ptr<Texture2D> LoadTexture(const std::string& name, const TextureFiltering filtering,
										   const TextureRepeat repeat);

	std::string LoadTextFile(const std::string& name);

	void SetMultiplier(const size_t multiplier)
	{
		mMultiplier = multiplier;
	}

private:
	
	size_t mMultiplier;
};

} // Pacman namespace
