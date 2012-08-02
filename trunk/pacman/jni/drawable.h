#pragma once

#include <memory>

namespace Pacman {

class VertexBuffer;
class Texture2D;
class ShaderProgram;

class Drawable
{
public:

	Drawable() = delete;
	Drawable(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<Texture2D> texture,
			 std::shared_ptr<ShaderProgram> shader);

	Drawable(const Drawable&) = default;
	virtual ~Drawable() {}

	Drawable& operator= (const Drawable&) = default;

	std::shared_ptr<VertexBuffer> GetVertexBuffer() const
	{
		return mVertexBuffer;
	}

	std::shared_ptr<Texture2D> GetTexture() const
	{
		return mTexture;
	}

	std::shared_ptr<ShaderProgram> GetShaderProgram() const
	{
		return mShaderProgram;
	}

	void SetVertexBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		mVertexBuffer = vertexBuffer;
	}

	void SetTexture(const std::shared_ptr<Texture2D> texture)
	{
		mTexture = texture;
	}

	void SetShaderProgram(const std::shared_ptr<ShaderProgram> shaderProgram)
	{
		mShaderProgram = shaderProgram;
	}

private:

	std::shared_ptr<VertexBuffer> mVertexBuffer;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
};

} // Pacman namespace
