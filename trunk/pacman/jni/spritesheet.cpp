#include "spritesheet.h"

#include "error.h"
#include "texture.h"
#include "shader_program.h"
#include "engine.h"
#include "asset_manager.h"

namespace Pacman {

SpriteSheet::SpriteSheet(const std::shared_ptr<Texture2D> texture, const NamedSpriteInfoArray& namedSpritesInfo)
           : mTexture(texture)
{
	for (const NamedSpriteInfo& namedInfo : namedSpritesInfo)
    {
        mSpritesInfo.insert(namedInfo);
    }
}

std::shared_ptr<Sprite> SpriteSheet::MakeSprite(const std::string& name, const SpriteRegion& region) const
{
    SpriteInfo info = GetSpriteInfo(name);
	AssetManager& assetManager = GetEngine()->GetAssetManager();

	std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(info.mVertexShaderName, info.mFragmentShaderName);
	return std::make_shared<Sprite>(region, info.mTextureRegion, mTexture, shaderProgram, info.mAlphaBlend);
}

SpriteInfo SpriteSheet::GetSpriteInfo(const std::string& name) const
{
    auto iter = mSpritesInfo.find(name);
    PACMAN_CHECK_ERROR(iter != mSpritesInfo.end(), ErrorCode::BadArgument);
    return iter->second;
}

} // Pacman namespace