#pragma once

#include <array>
#include <memory>
#include <vector>

#include "base.h"
#include "engine_forwdecl.h"
#include "engine_typedefs.h"
#include "drawable.h"
#include "color.h"

namespace Pacman {

class InstancedSprite : public IDrawable
{
public:

	InstancedSprite() = delete;

	InstancedSprite(const SpriteRegion& region, const Color leftTop, const Color rightTop, const Color leftBottom,
		            const Color rightBottom, const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend,
                    const std::vector<Position>& instances, const bool instanceEraseEnabled);

	InstancedSprite(const SpriteRegion& region, const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend,
                    const std::vector<Position>& instances, const bool instanceEraseEnabled);

	InstancedSprite(const SpriteRegion& region, const TextureRegion& textureRegion, const std::shared_ptr<Texture2D>& texture,
		            const std::shared_ptr<ShaderProgram>& shaderProgram, const bool alphaBlend, const std::vector<Position>& instances,
                    const bool instanceEraseEnabled);

	InstancedSprite(const SpriteRegion& region, const std::shared_ptr<Texture2D>& texture, const std::shared_ptr<ShaderProgram>& shaderProgram,
                    const bool alphaBlend, const std::vector<Position>& instances, const bool instanceEraseEnabled);

	InstancedSprite(const InstancedSprite&) = default;
	~InstancedSprite() = default;

	InstancedSprite& operator= (const InstancedSprite&) = default;

    void EraseInstance(const size_t index);

	virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() const;

	virtual std::weak_ptr<Texture2D> GetTexture() const;

	virtual std::shared_ptr<ShaderProgram> GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

private:

	void InitByColor(const SpriteRegion& region, const Color leftTop, const Color rightTop,
			   	   	 const Color leftBottom, const Color rightBottom, const std::vector<Position>& instances);

	void InitByTexture(const SpriteRegion& region, const TextureRegion& textureRegion, const std::vector<Position>& instances);

    void InitInstancesEraseState();

	std::shared_ptr<VertexBuffer>  mVertexBuffer;
	std::shared_ptr<Texture2D>     mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
    std::vector<bool>              mInstancesEraseStates; // true - erased, false - present
	bool                           mAlphaBlend;
    bool                           mInstanceEraseEnabled;
    size_t                         mInstancesCount;
};

} // Pacman namespace
