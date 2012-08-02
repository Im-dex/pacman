#include "sprite.h"
#include "base.h"
#include "texture.h"
#include "vertex_buffer.h"

#include <array>

namespace Pacman {

const size_t kVerticesCount = 6;
const size_t kPositionSize = sizeof(float) * 2;
const size_t kColorSize = sizeof(float) * 4;

template<size_t vertexSize, size_t vertexOffset>
class VerticesGenerator
{
public:

	VerticesGenerator() = delete;
	VerticesGenerator(const float spriteWidth, const float spriteHeight)
	{
		Generate(spriteWidth, spriteHeight);
	}
	VerticesGenerator(const VerticesGenerator&) = delete;
	~VerticesGenerator() = default;

	VerticesGenerator& operator= (const VerticesGenerator&) = delete;

	float* Get()
	{
		return mBuf.data();
	}

	size_t GetSize()
	{
		return mBufferSize;
	}

private:

	void Generate(const float spriteWidth, const float spriteHeight)
	{
		//
		// First triangle
		//

		// left top
		mBuf[0] = 0.0f;
		mBuf[1] = 0.0f;

		// left bottom
		mBuf[vertexOffset + vertexSize] 	= 0.0f;
		mBuf[vertexOffset + vertexSize + 1] = spriteHeight;

		// right bottom
		mBuf[vertexOffset*2 + vertexSize*2] 	= spriteWidth;
		mBuf[vertexOffset*2 + vertexSize*2 + 1] = spriteHeight;

		//
		// Second triangle
		//

		// left top
		mBuf[vertexOffset*3 + vertexSize*3] 	= 0.0f;
		mBuf[vertexOffset*3 + vertexSize*3 + 1] = 0.0f;

		// right bottom
		mBuf[vertexOffset*4 + vertexSize*4] 	= spriteWidth;
		mBuf[vertexOffset*4 + vertexSize*4 + 1] = spriteHeight;

		// right top
		mBuf[vertexOffset*5 + vertexSize*5] 	= spriteWidth;
		mBuf[vertexOffset*5 + vertexSize*5 + 1] = 0.0f;
	}

	enum
	{
		mBufferSize = (vertexSize + vertexOffset) * kVerticesCount
	};

	std::array<float, mBufferSize> mBuf;
};

//================================================================================================

FORCEINLINE void FillColor(float* buf, const Color& color)
{
	buf[0] = color.GetRedFloat();
	buf[1] = color.GetGreenFloat();
	buf[2] = color.GetBlueFloat();
	buf[3] = color.GetAlphaFloat();
}

Sprite::Sprite(const float width, const float height, const Color& leftTop, const Color& rightTop,
			   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram)
	  : Drawable(BuildVertexColorBuffer(width, height, leftTop, rightTop, leftBottom, rightBottom), std::shared_ptr<Texture2D>(), shaderProgram)
{
}

/*Sprite::Sprite(const float width, const float height, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram)
{

}*/

std::shared_ptr<VertexBuffer> Sprite::BuildVertexColorBuffer(const float width, const float height, const Color& leftTop, const Color& rightTop,
		   	   	   	   	   	   	   	   	   	   	     	 	 const Color& leftBottom, const Color& rightBottom)
{
	VerticesGenerator<kPositionSize, kColorSize> verticesGenerator(width, height);
	float* buf = verticesGenerator.Get();

	FillColor(buf + kPositionSize, 					leftTop);
	FillColor(buf + kPositionSize*2 + kColorSize,   rightBottom);
	FillColor(buf + kPositionSize*3 + kColorSize*2, leftBottom);
	FillColor(buf + kPositionSize*4 + kColorSize*3, leftTop);
	FillColor(buf + kPositionSize*5 + kColorSize*4, rightTop);
	FillColor(buf + kPositionSize*6 + kColorSize*5, rightBottom);

	const fvf_t fvf = static_cast<fvf_t>(FVFElement::Position) | static_cast<fvf_t>(FVFElement::Color);
	const void* bufPtr = static_cast<const void*>(buf);
	return std::make_shared<VertexBuffer>(static_cast<const byte_t*>(bufPtr), verticesGenerator.GetSize(), fvf);
}

} // Pacman namespace
