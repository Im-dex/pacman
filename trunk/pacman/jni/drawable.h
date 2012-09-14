#pragma once

#include <memory>

#include "base.h"

namespace Pacman {

class VertexBuffer;
class Texture2D;
class ShaderProgram;

class IDrawable
{
public:

	virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() const = 0;

	virtual std::weak_ptr<Texture2D> GetTexture() const = 0;

	virtual std::shared_ptr<ShaderProgram> GetShaderProgram() const = 0;

	virtual bool HasAlphaBlend() const = 0;
};

} // Pacman namespace
