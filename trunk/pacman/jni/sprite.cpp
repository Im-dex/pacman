#include "sprite.h"

namespace Pacman {

static const std::vector<SpritePosition> kPositions = std::vector<SpritePosition>(1, SpritePosition(0, 0));

Sprite::Sprite(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, const Color& leftBottom,
			   const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, leftTop, rightTop, leftBottom, rightBottom, shaderProgram, alphaBlend, kPositions, false)
{
}

Sprite::Sprite(const SpriteRegion& region, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, shaderProgram, alphaBlend, kPositions, false)
{
}

Sprite::Sprite(const SpriteRegion& region, const TextureRegion& textureRegion, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, textureRegion, texture, shaderProgram, alphaBlend, kPositions, false)
{
}

Sprite::Sprite(const SpriteRegion& region, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, texture, shaderProgram, alphaBlend, kPositions, false)
{
}

VertexBuffer& Sprite::GetVertexBuffer() const
{
	return mInstancedSprite.GetVertexBuffer();
}

std::weak_ptr<Texture2D> Sprite::GetTexture() const
{
	return mInstancedSprite.GetTexture();
}

ShaderProgram& Sprite::GetShaderProgram() const
{
	return mInstancedSprite.GetShaderProgram();
}

bool Sprite::HasAlphaBlend() const
{
	return mInstancedSprite.HasAlphaBlend();
}

} // Pacman namespace
