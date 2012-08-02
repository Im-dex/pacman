#pragma once

#include <GLES2/gl2.h>
#include <string>

namespace Pacman {

enum class ShaderType
{
	VERTEX,
	FRAGMENT
};

class Shader
{
public:

	Shader() = delete;
	Shader(const ShaderType type, const std::string& shaderSource);
	Shader(const Shader&) = delete;
	~Shader();

	Shader& operator= (const Shader&) = delete;

	void Compile();

	GLuint GetHandle()
	{
		return mShaderHandle;
	}

	void SetSource(const std::string& source)
	{
		mShaderSource = source;
		mIsCompiled = false;
	}

	bool IsCompiled()
	{
		return mIsCompiled;
	}

private:

	std::string mShaderSource;
	GLuint mShaderHandle;
	bool mIsCompiled;
};

} // Pacman namespace
