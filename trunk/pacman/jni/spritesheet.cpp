#include "spritesheet.h"
#include "texture.h"
#include "json_helper.h"

namespace Pacman {

SpriteSheet::SpriteSheet(const std::string& description, std::shared_ptr<Texture2D> texture)
{
	const Json::Value root = JsonHelper::ParseJson(description);
}

} // Pacman namespace