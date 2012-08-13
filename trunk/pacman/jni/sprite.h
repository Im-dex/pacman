#pragma once

#include "drawable.h"
#include "rect.h"
#include "color.h"
#include "math/vector2.h"

#include <array>
#include <memory>

namespace Pacman {

class Texture2D;
class ShaderProgram;
class VertexBuffer;
struct Vertex;

typedef Rect<float> TextureRegion;
typedef Rect<size_t> SpriteRegion;

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

	virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() const;

	virtual std::shared_ptr<Texture2D> GetTexture() const;

	virtual std::shared_ptr<ShaderProgram> GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

private:

	void InitByColor(const SpriteRegion& region, const Color& leftTop, const Color& rightTop,
			   	   	 const Color& leftBottom, const Color& rightBottom);

	void InitByTexture(const SpriteRegion& region, const TextureRegion& textureRegion);

	std::shared_ptr<VertexBuffer> mVertexBuffer;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
	bool mAlphaBlend;
};

} // Pacman namespace
