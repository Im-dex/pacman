#pragma once

#include "base.h"

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

class Texture2D
{
public:

	Texture2D() = delete;
	Texture2D(const size_t width, const size_t height, const byte_t* data,
			  const TextureFiltering filtering, const TextureRepeat repeat);

	Texture2D(const Texture2D&) = delete;
	~Texture2D();

	Texture2D& operator= (const Texture2D&) = delete;

	void Bind();

private:

	GLuint mTextureHandle;
};

} // Pacman namespace
