#pragma once

#include "base.h"
#include "math/vector2.h"
#include "color.h"

#include <memory>

namespace Pacman {

class ShaderProgram;

// type for storing FVFElement combinations
typedef uint8_t fvf_t;

// Flexible Vertex Format elements
enum class FVFElement : fvf_t
{
	None	  = 0,
	Position  = 1, // 2 float coordinates
	Color	  = 2, // 4 float RGBA components
	TexCoords = 4  // 2 float UV coordinates
};

class VertexBuffer
{
public:

	VertexBuffer();
	VertexBuffer(const byte_t* vertexData, const size_t size, const fvf_t fvf);
	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer();

	VertexBuffer& operator= (const VertexBuffer&) = delete;

	void SetData(const byte_t* vertexData, const size_t size, const fvf_t fvf);

	void Bind(const std::shared_ptr<ShaderProgram> shaderProgram) const;

	void Draw() const;

private:

	const size_t ComputeVertexSize(const fvf_t fvf) const;

	byte_t* mVertexData;
	size_t  mVertexDataSize;
	size_t  mVertexSize;
	fvf_t	mFVF;
};

} // Pacman namespace
