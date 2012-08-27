#pragma once

#include "sprite.h"
#include "json_helper.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace Pacman {

class Texture2D;

class SpriteSheet
{
public:

	SpriteSheet() = delete;
	SpriteSheet(const std::string& description);
	SpriteSheet(const SpriteSheet&) = default;
	~SpriteSheet() = default;

	SpriteSheet& operator= (const SpriteSheet&) = default;

	std::shared_ptr<Sprite> MakeSprite(const std::string& name, const SpriteRegion& region);

private:

	std::unordered_map<std::string, Json::Value> mSprites;
	std::shared_ptr<Texture2D>					 mTexture;
};

} // Pacman namespace