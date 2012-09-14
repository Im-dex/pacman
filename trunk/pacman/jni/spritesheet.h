#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "base.h"
#include "sprite.h"
#include "json_helper.h"

namespace Pacman {

class Texture2D;

struct SpriteInfo
{
    TextureRegion mTextureRegion;
    std::string   mVertexShaderName;
    std::string   mFragmentShaderName;
    bool          mAlphaBlend;
};

class SpriteSheet
{
public:

	SpriteSheet() = delete;
	SpriteSheet(const std::string& description);
	SpriteSheet(const SpriteSheet&) = default;
	~SpriteSheet() = default;

	SpriteSheet& operator= (const SpriteSheet&) = default;

	std::shared_ptr<Sprite> MakeSprite(const std::string& name, const SpriteRegion& region) const;

    SpriteInfo GetSpriteInfo(const std::string& name) const;

    std::shared_ptr<Texture2D> GetTexture() const
    {
        return mTexture;
    }

private:

	std::unordered_map<std::string, Json::Value> mSprites;
	std::shared_ptr<Texture2D>					 mTexture;
};

} // Pacman namespace