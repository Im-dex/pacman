#include "texture.h"
#include "error.h"

namespace Pacman {

Texture2D::Texture2D(const size_t width, const size_t height, const byte_t* data,
					 const TextureFiltering filtering, const TextureRepeat repeat)
{
	glGenTextures(1, &mTextureHandle);
	Bind();

	switch (filtering)
	{
	case TextureFiltering::Bilinear:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFiltering::Trilinear:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TextureFiltering::None:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	}

	switch (repeat)
	{
	case TextureRepeat::Repeat_S:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		break;
	case TextureRepeat::Repeat_T:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TextureRepeat::Repeat_ST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TextureRepeat::None:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	PACMAN_CHECK_GL_ERROR();
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mTextureHandle);
}

void Texture2D::Bind()
{
	glBindTexture(GL_TEXTURE_2D, mTextureHandle);
	PACMAN_CHECK_GL_ERROR();
}

} // Pacman namespace
