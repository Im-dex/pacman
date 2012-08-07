#pragma once

#include "base.h"
#include "unique_id.h"

#include <GLES2/gl2.h>

namespace Pacman {

enum class TextureFiltering
{
	None = 0,
	Bilinear,
	Trilinear
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

class Texture2D : public UniqueIdProvider
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

private:

	GLuint mTextureHandle;
};

} // Pacman namespace
