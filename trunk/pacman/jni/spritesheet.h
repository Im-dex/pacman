#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace Pacman {

class Sprite;

class SpriteSheet
{
public:

	SpriteSheet() = delete;
	SpriteSheet(const std::string& description);
	SpriteSheet(const SpriteSheet&) = default;
	~SpriteSheet() = default;

	SpriteSheet& operator= (const SpriteSheet&) = default;

	std::shared_ptr<Sprite> GetSprite(const std::string& name);

private:

	std::unordered_map<std::string, std::shared_ptr<Sprite>> mSprites;
};

} // Pacman namespace