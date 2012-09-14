#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "base.h"
#include "sprite.h"
#include "color.h"
#include "drawable.h"
#include "math/vector2.h"

namespace Pacman {

class Texture2D;
class ShaderProgram;

class FontManager
{
public:
	FontManager();
	FontManager(const FontManager&) = delete;
	~FontManager() = default;

	FontManager& operator= (const FontManager&) = delete;

	std::shared_ptr<Sprite> MakeString(const std::string& string, const Color textColor, const Math::Vector2f position) const;

private:

	size_t mASCIIstart;
	size_t mASCIIend;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
	std::vector<std::shared_ptr<Sprite>> mSprites;
};

} // Pacman namespace