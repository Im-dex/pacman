#pragma once

#include <string>
#include <memory>

namespace Pacman {

class Texture2D;

class SpriteSheet
{
public:

	SpriteSheet() = delete;
	SpriteSheet(const std::string& description, std::shared_ptr<Texture2D> texture);
	SpriteSheet(const SpriteSheet&) = default;
	~SpriteSheet() = default;

	SpriteSheet& operator= (const SpriteSheet&) = default;

private:

	std::shared_ptr<Texture2D> mTexture;
};

} // Pacman namespace