#include "vertex_buffer.h"
#include "error.h"

#include <cstring>
#include <cassert>
#include <cstddef>

namespace Pacman {

static GLenum ConvertUsage(const BufferUsage usage)
{
	switch (usage)
	{
	case BufferUsage::Static:
		return GL_STATIC_DRAW;
	case BufferUsage::Dynamic:
		return GL_DYNAMIC_DRAW;
	case BufferUsage::Stream:
		return GL_STREAM_DRAW;
	}

	PACMAN_CHECK_ERROR(false, ErrorCode::BadArgument);
}

//================================================================================================================================

VertexBuffer::VertexBuffer(const Vertex* vertexData, const uint16_t* indexData, const size_t vertexCount,
						   const size_t indexCount, const BufferUsage usage)
			: mIndexCount(indexCount),
			  mAttributesCount(3)
{
	const void* data = static_cast<const void*>(vertexData);
	Init(static_cast<const byte_t*>(data), indexData, sizeof(Vertex) * vertexCount, indexCount, usage);
	mVertexAttributes[0] = VertexAttribute(2, sizeof(Vertex), 0); // attrib #1
	mVertexAttributes[1] = VertexAttribute(4, sizeof(Vertex), offsetof(Vertex, r)); // attrib #2
	mVertexAttributes[2] = VertexAttribute(2, sizeof(Vertex), offsetof(Vertex, u)); // attrib #3
}

VertexBuffer::VertexBuffer(const ColorVertex* vertexData, const uint16_t* indexData, const size_t vertexCount,
						   const size_t indexCount, const BufferUsage usage)
			: mIndexCount(indexCount),
			  mAttributesCount(2)
{
	const void* data = static_cast<const void*>(vertexData);
	Init(static_cast<const byte_t*>(data), indexData, sizeof(ColorVertex) * vertexCount, indexCount, usage);
	mVertexAttributes[0] = VertexAttribute(2, sizeof(ColorVertex), 0); // attrib #1
	mVertexAttributes[1] = VertexAttribute(4, sizeof(ColorVertex), offsetof(ColorVertex, r)); // attrib #2
}

VertexBuffer::VertexBuffer(const TextureVertex* vertexData, const uint16_t* indexData, const size_t vertexCount,
						   const size_t indexCount, const BufferUsage usage)
			: mIndexCount(indexCount),
			  mAttributesCount(2)
{
	const void* data = static_cast<const void*>(vertexData);
	Init(static_cast<const byte_t*>(data), indexData, sizeof(TextureVertex) * vertexCount, indexCount, usage);
	mVertexAttributes[0] = VertexAttribute(2, sizeof(TextureVertex), 0); // attrib #1
	mVertexAttributes[1] = VertexAttribute(2, sizeof(TextureVertex), offsetof(TextureVertex, u)); // attrib #2
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(2, mBuffers.data());
}

void VertexBuffer::Bind() const
{
	assert(mAttributesCount > 0 && mAttributesCount <= GL_MAX_VERTEX_ATTRIBS);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	PACMAN_CHECK_GL_ERROR();

	for (size_t i = 0; i < mAttributesCount; i++)
	{
		VertexAttribute attribute = mVertexAttributes[i];

		glVertexAttribPointer(i, attribute.mComponentsCount, GL_FLOAT, GL_FALSE, attribute.mStride, reinterpret_cast<GLvoid*>(attribute.mBeginStride));
		glEnableVertexAttribArray(i);
		PACMAN_CHECK_GL_ERROR();
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	PACMAN_CHECK_GL_ERROR();
}

void VertexBuffer::Unbind() const
{
	for (size_t i = 0; i < mAttributesCount; i++)
	{
		glDisableVertexAttribArray(i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	PACMAN_CHECK_GL_ERROR();
}

void VertexBuffer::Draw() const
{
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0);
	PACMAN_CHECK_GL_ERROR();
}

void VertexBuffer::Init(const byte_t* vertexData, const uint16_t* indexData, const size_t vertexDataSize, const size_t indexCount, const BufferUsage usage)
{
	GLenum glUsage = ConvertUsage(usage);

	glGenBuffers(2, mBuffers.data());
	PACMAN_CHECK_GL_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	PACMAN_CHECK_GL_ERROR();
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, static_cast<const void*>(vertexData), glUsage);
	PACMAN_CHECK_GL_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	PACMAN_CHECK_GL_ERROR();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexCount, static_cast<const void*>(indexData), glUsage);
	PACMAN_CHECK_GL_ERROR();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // Pacman namespace
