#pragma once

#include "base.h"
#include "color.h"

#include <GLES2/gl2.h>
#include <array>
#include <vector>
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
	uint16_t x, y; // position
	float    r, g, b, a; // color
	float    u, v; // tex coords
};

struct ColorVertex 
{
	uint16_t x, y; // position
	float    r, g, b, a; // color
};

struct TextureVertex
{
	uint16_t x, y; // position
	float    u, v; // tex coords
};

class VertexBuffer
{
public:

	VertexBuffer() = delete;
	explicit VertexBuffer(const std::vector<Vertex>& vertexData, const std::vector<uint16_t>& indexData,
                          const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage);

	explicit VertexBuffer(const std::vector<ColorVertex>& vertexData, const std::vector<uint16_t>& indexData,
						  const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage);

	explicit VertexBuffer(const std::vector<TextureVertex>& vertexData, const std::vector<uint16_t>& indexData,
						  const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage);

	VertexBuffer(const VertexBuffer&) = delete;
	~VertexBuffer();

	VertexBuffer& operator= (const VertexBuffer&) = delete;

	void Bind() const;

	void Unbind() const;

	void Draw() const;

    std::vector<byte_t>& LockVertexData();

    void UnlockVertexData();

    std::vector<uint16_t>& LockIndexData();

    void UnlockIndexData();

    size_t GetVertexCount() const
    {
        return mVertexCount;
    }

    size_t GetIndexCount() const
    {
        return mIndexCount;
    }

    size_t GetAttributesCount() const
    {
        return mAttributesCount;
    }

private:

	void Init(const byte_t* vertexData, const size_t vertexDataSize, const std::vector<uint16_t>& indexData,
              const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage);

	struct VertexAttribute
	{
		size_t mComponentsCount;
		size_t mStride; // in bytes
		size_t mBeginStride; // in bytes
        GLenum mType;
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

	size_t                mIndexCount;
    size_t                mVertexCount;
	size_t                mAttributesCount;
    bool                  mVertexDataLocked;
    bool                  mIndexDataLocked;
    std::vector<byte_t>   mVertexCache;
    std::vector<uint16_t> mIndexCache;
	VertexAttributesArray mVertexAttributes;
};

} // Pacman namespace
