#pragma once

#include "drawable.h"
#include "color.h"
#include "engine_typedefs.h"

#include <array>
#include <memory>
#include <vector>
#include <cstdint>

namespace Pacman {

class Texture2D;
class ShaderProgram;
class VertexBuffer;
struct Vertex;

class InstancedSprite : public IDrawable
{
public:

	InstancedSprite() = delete;

	InstancedSprite(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, const Color& leftBottom,
		            const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend,
                    const std::vector<SpritePosition>& instances, const bool instanceHideEnabled);

	InstancedSprite(const SpriteRegion& region, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend,
                    const std::vector<SpritePosition>& instances, const bool instanceHideEnabled);

	InstancedSprite(const SpriteRegion& region, const TextureRegion& textureRegion, std::shared_ptr<Texture2D> texture,
		            std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend, const std::vector<SpritePosition>& instances,
                    const bool instanceHideEnabled);

	InstancedSprite(const SpriteRegion& region, std::shared_ptr<Texture2D> texture, std::shared_ptr<ShaderProgram> shaderProgram,
                    const bool alphaBlend, const std::vector<SpritePosition>& instances, const bool instanceHideEnabled);

	InstancedSprite(const InstancedSprite&) = default;
	~InstancedSprite() = default;

	InstancedSprite& operator= (const InstancedSprite&) = default;

    void HideInstance(const size_t index);

	virtual VertexBuffer& GetVertexBuffer() const;

	virtual std::weak_ptr<Texture2D> GetTexture() const;

	virtual ShaderProgram& GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

private:

	void InitByColor(const SpriteRegion& region, const Color& leftTop, const Color& rightTop,
			   	   	 const Color& leftBottom, const Color& rightBottom, const std::vector<SpritePosition>& instances);

	void InitByTexture(const SpriteRegion& region, const TextureRegion& textureRegion, const std::vector<SpritePosition>& instances);

	std::shared_ptr<VertexBuffer> mVertexBuffer;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
	bool   mAlphaBlend;
    bool   mInstanceHideEnabled;
    size_t mInstancesCount;
};

} // Pacman namespace
