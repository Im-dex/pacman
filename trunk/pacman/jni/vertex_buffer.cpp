#include "vertex_buffer.h"
#include "error.h"
#include "shader_program.h"

#include <cstring>

namespace Pacman {

static const size_t kPositionComponentsCount = 2; // x, y
static const size_t kColorComponentsCount = 4; // r, g, b, a
static const size_t kTexCoordsComponentsCount = 2; // u, v

static const char* kPositionAttributeName = "vPosition";
static const char* kColorAttributeName = "vColor";
static const char* kTexCoordsAttributeName = "vTexCoords";

VertexBuffer::VertexBuffer()
			: mVertices(),
			  mVerticesCount(0),
			  mFVF(static_cast<const fvf_t>(FVFElement::None))
{
}

VertexBuffer::VertexBuffer(const Vertex* vertexData, const size_t count, const fvf_t fvf)
			: mVertices(),
			  mVerticesCount(0),
			  mFVF(static_cast<const fvf_t>(FVFElement::None))
{
	SetData(vertexData, count, fvf);
}

void VertexBuffer::SetData(const Vertex* vertexData, const size_t count, const fvf_t fvf)
{
	mVertices = std::unique_ptr<Vertex[]>(new Vertex[count]);
	memcpy(mVertices.get(), vertexData, sizeof(Vertex) * count);
	mVerticesCount = count;
	mFVF = fvf;
}

void VertexBuffer::Bind(const std::shared_ptr<ShaderProgram> shaderProgram) const
{
	if ((mFVF & static_cast<const fvf_t>(FVFElement::Position)) == static_cast<const fvf_t>(FVFElement::Position))
	{
		const void* data = static_cast<const void*>(mVertices.get());
		shaderProgram->SetVertexAttribute(kPositionAttributeName, kPositionComponentsCount, VertexAttributeType::Float,
										  sizeof(Vertex), static_cast<const byte_t*>(data));
	}

	if ((mFVF & static_cast<const fvf_t>(FVFElement::Color)) == static_cast<const fvf_t>(FVFElement::Color))
	{
		const void* data = static_cast<const void*>(mVertices.get() + kPositionComponentsCount);
		shaderProgram->SetVertexAttribute(kColorAttributeName, kColorComponentsCount, VertexAttributeType::Float,
										  sizeof(Vertex), static_cast<const byte_t*>(data));
	}

	if ((mFVF & static_cast<const fvf_t>(FVFElement::TexCoords)) == static_cast<const fvf_t>(FVFElement::TexCoords))
	{
		const void* data = static_cast<const void*>(mVertices.get() + kPositionComponentsCount + kColorComponentsCount);
		shaderProgram->SetVertexAttribute(kTexCoordsAttributeName, kTexCoordsComponentsCount, VertexAttributeType::Float, sizeof(Vertex),
										  static_cast<const byte_t*>(data));
	}
}

void VertexBuffer::Draw() const
{
	glDrawArrays(GL_TRIANGLES, 0, mVerticesCount);
	PACMAN_CHECK_GL_ERROR();
}

} // Pacman namespace
