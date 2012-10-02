#include "vertex_buffer.h"

#include <cstring>
#include <cstddef>

#include "error.h"

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

VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertexData, const std::vector<uint16_t>& indexData,
						   const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage)
			: mVertexCount(vertexData.size()),
              mIndexCount(indexData.size()),
              mVertexDataLocked(false),
              mIndexDataLocked(false),
              mEmpty(false),
              mVertexCache(),
              mIndexCache(),
			  mAttributesCount(3)
{
	const void* data = static_cast<const void*>(&vertexData.front());
	Init(static_cast<const byte_t*>(data), sizeof(Vertex) * vertexData.size(), indexData, vertexBufferUsage, indexBufferUsage);
    mVertexAttributes[0] = { 2, sizeof(Vertex), 0, GL_UNSIGNED_SHORT }; // attrib #1
    mVertexAttributes[1] = { 4, sizeof(Vertex), offsetof(Vertex, r), GL_FLOAT }; // attrib #2
    mVertexAttributes[2] = { 2, sizeof(Vertex), offsetof(Vertex, u), GL_FLOAT }; // attrib #3
}

VertexBuffer::VertexBuffer(const std::vector<ColorVertex>& vertexData, const std::vector<uint16_t>& indexData,
						   const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage)
			: mVertexCount(vertexData.size()),
              mIndexCount(indexData.size()),
              mVertexDataLocked(false),
              mIndexDataLocked(false),
              mEmpty(false),
              mVertexCache(),
              mIndexCache(),
			  mAttributesCount(2)
{
	const void* data = static_cast<const void*>(&vertexData.front());
	Init(static_cast<const byte_t*>(data), sizeof(ColorVertex) * vertexData.size(), indexData, vertexBufferUsage, indexBufferUsage);
    mVertexAttributes[0] = { 2, sizeof(ColorVertex), 0, GL_UNSIGNED_SHORT }; // attrib #1
    mVertexAttributes[1] = { 4, sizeof(ColorVertex), offsetof(ColorVertex, r), GL_FLOAT }; // attrib #2
}

VertexBuffer::VertexBuffer(const std::vector<TextureVertex>& vertexData, const std::vector<uint16_t>& indexData,
						   const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage)
			: mVertexCount(vertexData.size()),
              mIndexCount(indexData.size()),
              mVertexDataLocked(false),
              mIndexDataLocked(false),
              mEmpty(false),
              mVertexCache(),
              mIndexCache(),
			  mAttributesCount(2)
{
	const void* data = static_cast<const void*>(&vertexData.front());
	Init(static_cast<const byte_t*>(data), sizeof(TextureVertex) * vertexData.size(), indexData, vertexBufferUsage, indexBufferUsage);
    mVertexAttributes[0] = { 2, sizeof(TextureVertex), 0, GL_UNSIGNED_SHORT }; // attrib #1
    mVertexAttributes[1] = { 2, sizeof(TextureVertex), offsetof(TextureVertex, u), GL_FLOAT }; // attrib #2
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(2, mBuffers.data());
}

void VertexBuffer::Bind() const
{
    PACMAN_CHECK_ERROR((mAttributesCount > 0) && (mAttributesCount <= GL_MAX_VERTEX_ATTRIBS), ErrorCode::InvalidState);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	PACMAN_CHECK_GL_ERROR();

	for (size_t i = 0; i < mAttributesCount; i++)
	{
		VertexAttribute attribute = mVertexAttributes[i];

		glVertexAttribPointer(i, attribute.mComponentsCount, attribute.mType, GL_FALSE, attribute.mStride, reinterpret_cast<GLvoid*>(attribute.mBeginStride));
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
    if (mEmpty)
        return;

    PACMAN_CHECK_ERROR2(!mVertexDataLocked && !mIndexDataLocked, ErrorCode::InvalidState, "one of the streams is locked now");
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0);
	PACMAN_CHECK_GL_ERROR();
}

std::vector<byte_t>& VertexBuffer::LockVertexData()
{
    PACMAN_CHECK_ERROR2(mVertexCache.size() > 0, ErrorCode::InvalidState, "vertex data isn't dynamic");
    mVertexDataLocked = true;
    return mVertexCache;
}

void VertexBuffer::UnlockVertexData(const size_t newVertexCount)
{
    PACMAN_CHECK_ERROR2(mVertexDataLocked, ErrorCode::InvalidState, "vertex stream isn't locked");

    if (newVertexCount != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        PACMAN_CHECK_GL_ERROR();
        glBufferData(GL_ARRAY_BUFFER, mVertexCache.size(), static_cast<const void*>(&mVertexCache.front()), GL_DYNAMIC_DRAW);
        PACMAN_CHECK_GL_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
        mEmpty = true;
    }
    
    mVertexDataLocked = false;
    mVertexCount = newVertexCount;
}

std::vector<uint16_t>& VertexBuffer::LockIndexData()
{
    PACMAN_CHECK_ERROR2(mIndexCache.size() > 0, ErrorCode::InvalidState, "index data isn't dynamic");
    mIndexDataLocked = true;
    return mIndexCache;
}

void VertexBuffer::UnlockIndexData()
{
    PACMAN_CHECK_ERROR2(mIndexDataLocked, ErrorCode::InvalidState, "index stream isn't locked");

    if (mIndexCache.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        PACMAN_CHECK_GL_ERROR();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * mIndexCache.size(), static_cast<const void*>(&mIndexCache.front()), GL_DYNAMIC_DRAW);
        PACMAN_CHECK_GL_ERROR();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        mEmpty = true;
    }
    
    mIndexDataLocked = false;
    mIndexCount = mIndexCache.size();
}

void VertexBuffer::Init(const byte_t* vertexData, const size_t vertexDataSize, const std::vector<uint16_t>& indexData,
                        const BufferUsage vertexBufferUsage, const BufferUsage indexBufferUsage)
{
	GLenum glVertexBufUsage = ConvertUsage(vertexBufferUsage);
    GLenum glIndexBufUsage = ConvertUsage(indexBufferUsage);

	glGenBuffers(2, mBuffers.data());
	PACMAN_CHECK_GL_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	PACMAN_CHECK_GL_ERROR();
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, static_cast<const void*>(vertexData), glVertexBufUsage);
	PACMAN_CHECK_GL_ERROR();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	PACMAN_CHECK_GL_ERROR();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexData.size(), static_cast<const void*>(&indexData.front()), glIndexBufUsage);
	PACMAN_CHECK_GL_ERROR();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // make a cache for dynamic data
    if (vertexBufferUsage == BufferUsage::Dynamic)
    {
        mVertexCache = std::vector<byte_t>(vertexData, vertexData + vertexDataSize);
    }

    if (indexBufferUsage == BufferUsage::Dynamic)
    {
        mIndexCache = indexData;
    }
}

} // Pacman namespace
