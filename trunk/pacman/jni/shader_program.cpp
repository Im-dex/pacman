#include "shader_program.h"
#include "error.h"

#include <memory>

namespace Pacman {

ShaderProgram::ShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
			 : mVertexShader(ShaderType::VERTEX, vertexShaderSource),
			   mFragmentShader(ShaderType::FRAGMENT, fragmentShaderSource),
			   mIsLinked(false),
			   mAttributeUniformHandles()
{
	mProgramHandle = glCreateProgram();
	PACMAN_CHECK_GL_ERROR();
	PACMAN_CHECK_ERROR(mProgramHandle != 0, ErrorCode::CreateShaderProgram);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(mProgramHandle);
}

void ShaderProgram::SetVertexShader(const std::string& shaderSource)
{
	mVertexShader.SetSource(shaderSource);
	mIsLinked = false;
}

void ShaderProgram::SetFragmentShader(const std::string& shaderSource)
{
	mFragmentShader.SetSource(shaderSource);
	mIsLinked = false;
}

void ShaderProgram::Link()
{
	if (mIsLinked)
		return;

	if (!mVertexShader.IsCompiled())
		mVertexShader.Compile();

	if (!mFragmentShader.IsCompiled())
		mFragmentShader.Compile();

	glAttachShader(mProgramHandle, mVertexShader.GetHandle());
	PACMAN_CHECK_GL_ERROR();

	glAttachShader(mProgramHandle, mFragmentShader.GetHandle());
	PACMAN_CHECK_GL_ERROR();

	glLinkProgram(mProgramHandle);

	GLint linkStatus = GL_FALSE;
	glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE)
	{
		GLint infoLength = 0;
	    glGetProgramiv(mProgramHandle, GL_INFO_LOG_LENGTH, &infoLength);
	    if (infoLength > 0)
	    {
	    	std::unique_ptr<char[]> buf(new char[infoLength]);
	        glGetProgramInfoLog(mProgramHandle, infoLength, nullptr, buf.get());
	        PACMAN_CHECK_ERROR2(false, ErrorCode::LinkShaderProgram, buf.get());
	    }
	    else
	    {
	    	PACMAN_CHECK_ERROR(false, ErrorCode::LinkShaderProgram);
	    }
	}

	mAttributeUniformHandles.clear();
	mIsLinked = true;
}

void ShaderProgram::Bind() const
{
	glUseProgram(mProgramHandle);
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::Unbind() const
{
	glUseProgram(0);
	PACMAN_CHECK_GL_ERROR();
}

auto ConvertVertexAttributeType(const VertexAttributeType type) -> decltype(GL_FIXED)
{
	switch (type)
	{
	case VertexAttributeType::Byte:
		return GL_BYTE;
	case VertexAttributeType::UByte:
		return GL_UNSIGNED_BYTE;
	case VertexAttributeType::Short:
		return GL_SHORT;
	case VertexAttributeType::UShort:
		return GL_UNSIGNED_SHORT;
	case VertexAttributeType::Float:
		return GL_FLOAT;
	case VertexAttributeType::Fixed:
		return GL_FIXED;
	};
}

void ShaderProgram::SetVertexAttribute(const std::string& attrName, const size_t count, const VertexAttributeType attrType,
									   const size_t offset, const byte_t* data) const
{
	GLint attrHandle = GetLocation(attrName, true);
	glVertexAttribPointer(attrHandle, count, ConvertVertexAttributeType(attrType), GL_FALSE, offset, data);
	PACMAN_CHECK_GL_ERROR();
	glEnableVertexAttribArray(attrHandle);
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const float value) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform1f(uniformHandle, value);
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Vector2f& vector) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform2f(uniformHandle, vector.GetX(), vector.GetY());
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Vector3f& vector) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform3f(uniformHandle, vector.GetX(), vector.GetY(), vector.GetZ());
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Vector4f& vector) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform4f(uniformHandle, vector.GetX(), vector.GetY(), vector.GetZ(), vector.GetW());
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const int32_t value) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform1i(uniformHandle, value);
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Vector2<int32_t>& vector) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform2i(uniformHandle, vector.GetX(), vector.GetY());
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Vector3<int32_t>& vector) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform3i(uniformHandle, vector.GetX(), vector.GetY(), vector.GetZ());
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Vector4<int32_t>& vector) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniform4i(uniformHandle, vector.GetX(), vector.GetY(), vector.GetZ(), vector.GetW());
	PACMAN_CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Math::Matrix4f& matrix) const
{
	GLint uniformHandle = GetLocation(uniformName, false);
	glUniformMatrix4fv(uniformHandle, 1, GL_FALSE, matrix.GetRawData());
	PACMAN_CHECK_GL_ERROR();
}

GLint ShaderProgram::GetLocation(const std::string& name, const bool attribute) const
{
	auto iter = mAttributeUniformHandles.find(name);
	if (iter == mAttributeUniformHandles.end())
	{
		GLint handle = attribute ? glGetAttribLocation(mProgramHandle, name.c_str())
								 : glGetUniformLocation(mProgramHandle, name.c_str());
		PACMAN_CHECK_GL_ERROR();
		PACMAN_CHECK_ERROR2(handle != -1, ErrorCode::ShaderLocationSearch, name.c_str());
		auto result = mAttributeUniformHandles.insert(std::make_pair(name, handle));
		PACMAN_CHECK_ERROR(result.second, ErrorCode::ContainerInsert);
		iter = result.first;
	}

	return iter->second;
}

} // Pacman namespace
