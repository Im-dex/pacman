#pragma once

#include "drawable.h"
#include "rect.h"
#include "color.h"

#include <array>
#include <memory>
#include <vector>
#include <cstdint>

namespace Pacman {

class Texture2D;
class ShaderProgram;
class VertexBuffer;
struct Vertex;

typedef Rect<float> TextureRegion;
typedef Rect<uint16_t> SpriteRegion;
typedef SpriteRegion::Position SpritePosition;

class InstancedSprite : public IDrawable
{
public:

	InstancedSprite() = delete;

	InstancedSprite(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, const Color& leftBottom,
		            const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend,
                    const std::vector<SpritePosition>& positions, const bool instanceHideEnabled);

	InstancedSprite(const SpriteRegion& region, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend,
                    const std::vector<SpritePosition>& positions, const bool instanceHideEnabled);

	InstancedSprite(const SpriteRegion& region, const TextureRegion& textureRegion, std::shared_ptr<Texture2D> texture,
		            std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend, const std::vector<SpritePosition>& positions,
                    const bool instanceHideEnabled);

	InstancedSprite(const SpriteRegion& region, std::shared_ptr<Texture2D> texture, std::shared_ptr<ShaderProgram> shaderProgram,
                    const bool alphaBlend, const std::vector<SpritePosition>& positions, const bool instanceHideEnabled);

	InstancedSprite(const InstancedSprite&) = default;
	~InstancedSprite() = default;

	InstancedSprite& operator= (const InstancedSprite&) = default;

	virtual VertexBuffer& GetVertexBuffer() const;

	virtual std::weak_ptr<Texture2D> GetTexture() const;

	virtual ShaderProgram& GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

private:

	void InitByColor(const SpriteRegion& region, const Color& leftTop, const Color& rightTop,
			   	   	 const Color& leftBottom, const Color& rightBottom, const std::vector<SpritePosition>& positions);

	void InitByTexture(const SpriteRegion& region, const TextureRegion& textureRegion, const std::vector<SpritePosition>& positions);

	std::shared_ptr<VertexBuffer> mVertexBuffer;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
	bool mAlphaBlend;
};

} // Pacman namespace
