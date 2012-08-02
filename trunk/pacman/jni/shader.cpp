#include "shader.h"
#include "error.h"
#include "base.h"

#include <memory>

namespace Pacman {

Shader::Shader(const ShaderType type, const std::string& shaderSource)
	  : mShaderSource(shaderSource),
	    mIsCompiled(false)
{
	GLenum glType = (type == ShaderType::VERTEX) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	mShaderHandle = glCreateShader(glType);
	PACMAN_CHECK_GL_ERROR();
	PACMAN_CHECK_ERROR(mShaderHandle != 0, ErrorCode::CreateShader);
}

Shader::~Shader()
{
	glDeleteShader(mShaderHandle);
}

void Shader::Compile()
{
	const char* source = mShaderSource.c_str();
	glShaderSource(mShaderHandle, 1, &source, nullptr);
	PACMAN_CHECK_GL_ERROR();
	glCompileShader(mShaderHandle);

	GLint isCompiled = GL_FALSE;
	glGetShaderiv(mShaderHandle, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled != GL_TRUE)
	{
		GLint infoLength = 0;
		glGetShaderiv(mShaderHandle, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength > 0)
		{
			std::unique_ptr<char[]> buf(new char[infoLength]);
			glGetShaderInfoLog(mShaderHandle, infoLength, nullptr, buf.get());
			PACMAN_CHECK_ERROR2(false, ErrorCode::CompileShader, buf.get());
		}
		else
		{
			PACMAN_CHECK_ERROR(false, ErrorCode::CompileShader);
		}
	}

	mIsCompiled = true;
}

} // Pacman namespace
