#pragma once

#include <memory>

namespace Pacman {

class VertexBuffer;
class Texture2D;
class ShaderProgram;

class IDrawable
{
public:

	virtual VertexBuffer& GetVertexBuffer() const = 0;

	virtual std::weak_ptr<Texture2D> GetTexture() const = 0;

	virtual ShaderProgram& GetShaderProgram() const = 0;

	virtual bool HasAlphaBlend() const = 0;
};

} // Pacman namespace
