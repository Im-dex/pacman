#pragma once

#include "base.h"
#include "math/vector2.h"
#include "color.h"

#include <GLES2/gl2.h>
#include <array>
#include <cstdint>

namespace Pacman {

static const size_t kMaxVertexAttributesCount = 3;

enum class BufferUsage
{
	Static,
	Dynamic,
	Stream
};

struct Vertex
{
	float x, y; // position
	float r, g, b, a; // color
	float u, v; // tex coords
};

struct ColorVertex 
{
	float x, y; // position
	float r, g, b, a; // color
};

struct TextureVertex
{
	float x, y; // position
	float u, v; // tex coords
};

class VertexBuffer
{
public:

	VertexBuffer() = delete;
	explicit VertexBuffer(const Vertex* vertexData, const uint16_t* indexData, const size_t vertexCount,
						  const size_t indexCount, const BufferUsage usage);

	explicit VertexBuffer(const ColorVertex* vertexData, const uint16_t* indexData, const size_t vertexCount,
						  const size_t indexCount, const BufferUsage usage);

	explicit VertexBuffer(const TextureVertex* vertexData, const uint16_t* indexData, const size_t vertexCount,
						  const size_t indexCount, const BufferUsage usage);

	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer();

	VertexBuffer& operator= (const VertexBuffer&) = delete;

	void Bind() const;

	void Unbind() const;

	void Draw() const;

private:

	void Init(const byte_t* vertexData, const uint16_t* indexData, const size_t vertexDataSize, const size_t indexCount, const BufferUsage usage);

	struct VertexAttribute
	{
		VertexAttribute()
			: mComponentsCount(0),
			  mStride(0),
			  mBeginStride(0)
		{
		}
		
		VertexAttribute(const size_t componentsCount, const size_t stride, const size_t beginStride)
			: mComponentsCount(componentsCount),
			  mStride(stride),
			  mBeginStride(beginStride)
		{}

		VertexAttribute(const VertexAttribute&) = default;
		~VertexAttribute() = default;

		VertexAttribute& operator= (const VertexAttribute&) = default;

		size_t mComponentsCount;
		size_t mStride; // in bytes
		size_t mBeginStride; // in bytes
	};

	typedef std::array<VertexAttribute, kMaxVertexAttributesCount> VertexAttributesArray;

	union
	{
		struct
		{
			GLuint mVertexBuffer;
			GLuint mIndexBuffer;
		};
		std::array<GLuint, 2> mBuffers;
	};

	size_t mIndexCount;
	size_t mAttributesCount;
	VertexAttributesArray mVertexAttributes;
};

} // Pacman namespace
