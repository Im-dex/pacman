#pragma once

#include <memory>

#include "base.h"
#include "engine_forwdecl.h"

namespace Pacman {

class IDrawable
{
public:

	virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() const = 0;

	virtual std::weak_ptr<Texture2D> GetTexture() const = 0;

	virtual std::shared_ptr<ShaderProgram> GetShaderProgram() const = 0;

	virtual bool HasAlphaBlend() const = 0;
};

} // Pacman namespace
