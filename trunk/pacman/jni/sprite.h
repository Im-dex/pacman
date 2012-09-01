#pragma once

#include "instanced_sprite.h"

namespace Pacman {

class Sprite : public IDrawable
{
public:

	Sprite() = delete;

	Sprite(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, const Color& leftBottom,
		   const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend);

	Sprite(const SpriteRegion& region, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend);

	Sprite(const SpriteRegion& region, const TextureRegion& textureRegion, std::shared_ptr<Texture2D> texture,
		   std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend);

	Sprite(const SpriteRegion& region, std::shared_ptr<Texture2D> texture,
		   std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend);

	Sprite(const Sprite&) = default;
	~Sprite() = default;

	Sprite& operator= (const Sprite&) = default;

	virtual VertexBuffer& GetVertexBuffer() const;

	virtual std::weak_ptr<Texture2D> GetTexture() const;

	virtual ShaderProgram& GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

private:

    InstancedSprite mInstancedSprite;
};

} // Pacman namespace
