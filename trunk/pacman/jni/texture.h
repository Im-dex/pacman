#pragma once

#include <GLES2/gl2.h>

#include "base.h"

namespace Pacman {

enum class TextureFiltering : uint8_t
{
	None      = 0,
	Bilinear  = 1,
	Trilinear = 2
};

enum class TextureRepeat
{
	None = 0,
	Repeat_S,
	Repeat_T,
	Repeat_ST
};

enum class PixelFormat
{
	None,
	RGB_565,
	RGB_888,
	RGBA_8888,
	RGBA_4444,
	A_8
};

class Texture2D
{
public:

	Texture2D() = delete;
	Texture2D(const size_t width, const size_t height, const byte_t* data,
			  const TextureFiltering filtering, const TextureRepeat repeat,
			  const PixelFormat pixelFormat);

	Texture2D(const Texture2D&) = delete;
	~Texture2D();

	Texture2D& operator= (const Texture2D&) = delete;

	void Bind() const;

	void Unbind() const;

	size_t GetWidth() const
	{
		return mWidth;
	}

	size_t GetHeight() const
	{
		return mHeight;
	}

private:

	GLuint mTextureHandle;
	size_t mWidth;
	size_t mHeight;
};

} // Pacman namespace
