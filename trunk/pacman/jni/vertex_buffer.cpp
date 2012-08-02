#include "vertex_buffer.h"
#include "error.h"
#include "shader_program.h"

#include <cstring>

namespace Pacman {

const size_t kPositionComponentsCount = 2;
const size_t kColorComponentsCount = 4;
const size_t kTexCoordComponentsCount = 2;

const size_t kPositionSize = sizeof(float) * kPositionComponentsCount; // 2 coordinates for 2D
const size_t kColorSize = sizeof(float) * kColorComponentsCount; // 4 color components (include alpha)
const size_t kTexCoordsSize = sizeof(float) * kTexCoordComponentsCount; // UV

static const char* kPositionAttributeName = "vPosition";
static const char* kColorAttributeName = "vColor";
static const char* kTexCoordsAttributeName = "vTexCoords";

VertexBuffer::VertexBuffer()
			: mVertexData(nullptr),
			  mVertexDataSize(0),
			  mVertexSize(0),
			  mFVF(static_cast<const fvf_t>(FVFElement::None))
{
}

VertexBuffer::VertexBuffer(const byte_t* vertexData, const size_t size, const fvf_t fvf)
			: mVertexData(nullptr),
			  mVertexDataSize(0),
			  mVertexSize(0),
			  mFVF(static_cast<const fvf_t>(FVFElement::None))
{
	SetData(vertexData, size, fvf);
}

VertexBuffer::~VertexBuffer()
{
	delete[] mVertexData;
}

void VertexBuffer::SetData(const byte_t* vertexData, const size_t size, const fvf_t fvf)
{
	delete[] mVertexData;
	mVertexData = new byte_t[size];
	memcpy(mVertexData, vertexData, size);
	mVertexDataSize = size;
	mVertexSize = ComputeVertexSize(fvf);
	mFVF = fvf;
	PACMAN_CHECK_ERROR(mVertexDataSize % mVertexSize == 0, ErrorCode::BadFVF);
}

void VertexBuffer::Bind(const std::shared_ptr<ShaderProgram> shaderProgram) const
{
	size_t beginingOffset = 0;

	if ((mFVF & static_cast<const fvf_t>(FVFElement::Position)) == static_cast<const fvf_t>(FVFElement::Position))
	{
		shaderProgram->SetVertexAttribute(kPositionAttributeName, kPositionComponentsCount, VertexAttributeType::Float,
										  mVertexSize - kPositionSize, mVertexData);
		beginingOffset += kPositionSize;
	}

	if ((mFVF & static_cast<const fvf_t>(FVFElement::Color)) == static_cast<const fvf_t>(FVFElement::Color))
	{
		shaderProgram->SetVertexAttribute(kColorAttributeName, kColorComponentsCount, VertexAttributeType::Float,
										  mVertexSize - kColorSize, mVertexData + beginingOffset);
		beginingOffset += kColorSize;
	}

	if ((mFVF & static_cast<const fvf_t>(FVFElement::TexCoords)) == static_cast<const fvf_t>(FVFElement::TexCoords))
	{
		shaderProgram->SetVertexAttribute(kTexCoordsAttributeName, kTexCoordComponentsCount, VertexAttributeType::Float,
										  mVertexSize - kTexCoordsSize, mVertexData + beginingOffset);
		beginingOffset += kTexCoordsSize;
	}
}

void VertexBuffer::Draw() const
{
	glDrawArrays(GL_TRIANGLES, 0, mVertexDataSize / mVertexSize);
	PACMAN_CHECK_GL_ERROR();
}

const size_t VertexBuffer::ComputeVertexSize(const fvf_t fvf) const
{
	size_t result = 0;
	if ((fvf & static_cast<const fvf_t>(FVFElement::Position)) == static_cast<const fvf_t>(FVFElement::Position))
		result += kPositionSize;

	if ((fvf & static_cast<const fvf_t>(FVFElement::Color)) == static_cast<const fvf_t>(FVFElement::Color))
		result += kColorSize;

	if ((fvf & static_cast<const fvf_t>(FVFElement::TexCoords)) == static_cast<const fvf_t>(FVFElement::TexCoords))
		result += kTexCoordsSize;
	return result;
}

} // Pacman namespace
