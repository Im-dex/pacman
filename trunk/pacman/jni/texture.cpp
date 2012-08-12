#include "texture.h"
#include "error.h"

namespace Pacman {

Texture2D::Texture2D(const size_t width, const size_t height, const byte_t* data,
					 const TextureFiltering filtering, const TextureRepeat repeat,
					 const PixelFormat pixelFormat)
		 : mWidth(width),
		   mHeight(height)
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

	GLint format = -1;
	GLenum type = -1;

	switch (pixelFormat)
	{
	case PixelFormat::RGB_565:
		format = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case PixelFormat::RGB_888:
		format = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		break;
	case PixelFormat::RGBA_8888:
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case PixelFormat::RGBA_4444:
		format = GL_RGBA;
		type = GL_UNSIGNED_SHORT_4_4_4_4;
		break;
	case PixelFormat::A_8:
		format = GL_ALPHA;
		type = GL_UNSIGNED_BYTE;
		break;
	case PixelFormat::None:
		PACMAN_CHECK_ERROR(false, ErrorCode::BadPixelFormat);
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
	PACMAN_CHECK_GL_ERROR();
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mTextureHandle);
}

void Texture2D::Bind() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureHandle);
	PACMAN_CHECK_GL_ERROR();
}

} // Pacman namespace
