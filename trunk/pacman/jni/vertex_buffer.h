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

struct Vertex
{
	float x, y; // position
	float r, g, b, a; // color
	float u, v; // tex coords
};

class VertexBuffer
{
public:

	VertexBuffer();
	VertexBuffer(const Vertex* vertexData, const size_t count, const fvf_t fvf);
	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer() = default;

	VertexBuffer& operator= (const VertexBuffer&) = delete;

	void SetData(const Vertex* vertexData, const size_t count, const fvf_t fvf);

	void Bind(const std::shared_ptr<ShaderProgram> shaderProgram) const;

	void Draw() const;

private:

	std::unique_ptr<Vertex[]> mVertices;
	size_t  				  mVerticesCount;
	fvf_t					  mFVF;
};

} // Pacman namespace
