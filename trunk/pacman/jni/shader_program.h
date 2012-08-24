#pragma once

#include "base.h"
#include "shader.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4.h"
#include "math/matrix4.h"

#include <GLES2/gl2.h>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Pacman {

enum class VertexAttributeType
{
	Byte,
	UByte,
	Short,
	UShort,
	Float,
	Fixed
};

class ShaderProgram
{
public:

	// attrs: position, color; uniforms: modelView
	static const std::string kDefaultColorVertexShader;
	// attrs: position, color; uniforms: none
	static const std::string kDefaultStaticColorVertexShader;
	// varying: color
	static const std::string kDefaultColorFragmentShader;
	// attrs: position, texcoords; uniforms: modelView
	static const std::string kDefaultTextureVertexShader;
	// attrs: position, texcoords; uniforms: none
	static const std::string kDefaultStaticTextureVertexShader;
	// varying: texcoords
	static const std::string kDefaultTextureFragmentShader;

	ShaderProgram() = delete;
	ShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram();

	ShaderProgram& operator= (const ShaderProgram&) = delete;

	void SetVertexShader(const std::string& shaderSource);
	void SetFragmentShader(const std::string& shaderSource);

	void Link();

	void Bind() const;

	void Unbind() const;

	// attrName - attribute name
	// count - number of values per vertex component
	// attrType - attribute type
	// offset - offset between attribute components
	// data - data pointer
	void SetVertexAttribute(const std::string& attrName, const size_t count, const VertexAttributeType attrType,
							const size_t offset, const byte_t* data) const;

	void SetUniform(const std::string& uniformName, const float value) const;
	void SetUniform(const std::string& uniformName, const Math::Vector2f& vector) const;
	void SetUniform(const std::string& uniformName, const Math::Vector3f& vector) const;
	void SetUniform(const std::string& uniformName, const Math::Vector4f& vector) const;

	void SetUniform(const std::string& uniformName, const int32_t value) const;
	void SetUniform(const std::string& uniformName, const Math::Vector2<int32_t>& vector) const;
	void SetUniform(const std::string& uniformName, const Math::Vector3<int32_t>& vector) const;
	void SetUniform(const std::string& uniformName, const Math::Vector4<int32_t>& vector) const;

	void SetUniform(const std::string& uniformName, const Math::Matrix4f& matrix) const;

private:

	GLint GetLocation(const std::string& name, const bool attribute) const;

	Shader mVertexShader;
	Shader mFragmentShader;
	GLuint mProgramHandle;
	bool mIsLinked;
	mutable std::unordered_map<std::string, GLint> mAttributeUniformHandles;
};

} // Pacman namespace
