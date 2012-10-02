#include "sprite.h"

namespace Pacman {

static const std::vector<Position> kInstance = std::vector<Position>(1, Position::kZero);

Sprite::Sprite(const SpriteRegion& region, const Color leftTop, const Color rightTop, const Color leftBottom,
			   const Color rightBottom, const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, leftTop, rightTop, leftBottom, rightBottom, shaderProgram, alphaBlend, kInstance, false)
{
}

Sprite::Sprite(const SpriteRegion& region, const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, shaderProgram, alphaBlend, kInstance, false)
{
}

Sprite::Sprite(const SpriteRegion& region, const TextureRegion& textureRegion, const std::shared_ptr<Texture2D>& texture,
			   const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, textureRegion, texture, shaderProgram, alphaBlend, kInstance, false)
{
}

Sprite::Sprite(const SpriteRegion& region, const std::shared_ptr<Texture2D>& texture,
			   const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend)
	  : mInstancedSprite(region, texture, shaderProgram, alphaBlend, kInstance, false)
{
}

std::shared_ptr<VertexBuffer> Sprite::GetVertexBuffer() const
{
	return mInstancedSprite.GetVertexBuffer();
}

std::weak_ptr<Texture2D> Sprite::GetTexture() const
{
	return mInstancedSprite.GetTexture();
}

std::shared_ptr<ShaderProgram> Sprite::GetShaderProgram() const
{
	return mInstancedSprite.GetShaderProgram();
}

bool Sprite::HasAlphaBlend() const
{
	return mInstancedSprite.HasAlphaBlend();
}

} // Pacman namespace
