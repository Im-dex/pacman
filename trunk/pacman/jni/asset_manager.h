#pragma once

#include "base.h"
#include "texture.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Pacman {

class AssetManager
{
public:

	AssetManager();
	AssetManager(const AssetManager&) = delete;
	~AssetManager() = default;

	AssetManager& operator= (const AssetManager&) = delete;

	std::shared_ptr<Texture2D> LoadTexture(const char* name, const TextureFiltering filtering,
										   const TextureRepeat repeat);

	std::string LoadTextFile(const char* name);

	std::string LoadTextFileFromRoot(const char* name);

	uint8_t GetTileSize() const
	{
		return mTileSize;
	}

private:

	std::string BuildAssetPath(const char* name);

	uint8_t mTileSize;
	std::string mAssetFolder;
};

} // Pacman namespace
