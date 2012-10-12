#pragma once

#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <unordered_map>

#include "base.h"
#include "engine_forwdecl.h"
#include "sprite.h"

namespace Pacman {

struct SpriteInfo
{
    TextureRegion mTextureRegion;
    std::string   mVertexShaderName;
    std::string   mFragmentShaderName;
    bool          mAlphaBlend;
};

typedef std::pair<std::string, SpriteInfo> NamedSpriteInfo;
typedef std::vector<NamedSpriteInfo> NamedSpriteInfoArray; 

class SpriteSheet
{
public:

	SpriteSheet() = delete;
	SpriteSheet(const std::shared_ptr<Texture2D>& texture, const NamedSpriteInfoArray& namedSpritesInfo);
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

	std::unordered_map<std::string, SpriteInfo>  mSpritesInfo;
	std::shared_ptr<Texture2D>					 mTexture;
};

} // Pacman namespace