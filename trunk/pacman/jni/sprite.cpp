#include "sprite.h"
#include "base.h"
#include "texture.h"
#include "vertex_buffer.h"

namespace Pacman {

static const Color kDefaultColor = Color::kGreen;

static const Math::Vector2f kDefaultLeftTopTexCoord = Math::Vector2f::kZero;
static const Math::Vector2f kDefaultRightTopTexCoord = Math::Vector2f(1.0f, 0.0f);
static const Math::Vector2f kDefaultLeftBottomTexCoord = Math::Vector2f(0.0f, 1.0f);
static const Math::Vector2f kDefaultRightBottomTexCoord = Math::Vector2f(1.0f, 1.0f);

static FORCEINLINE void FillColor(ColorVertex& vertex, const Color& color)
{
	vertex.r = color.GetRedFloat();
	vertex.g = color.GetGreenFloat();
	vertex.b = color.GetBlueFloat();
	vertex.a = color.GetAlphaFloat();
}

static FORCEINLINE void FillTexCoord(TextureVertex& vertex, const Math::Vector2f texCoord)
{
	vertex.u = texCoord.GetX();
	vertex.v = texCoord.GetY();
}

//===========================================================================================================

Sprite::Sprite(const size_t width, const size_t height, const Color& leftTop, const Color& rightTop,
			   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram)
	  : mShaderProgram(shaderProgram),
	    mTexture(nullptr),
		mVertexBuffer(nullptr)
{
	InitByColor(width, height, leftTop, rightTop, leftBottom, rightBottom);
}

Sprite::Sprite(const size_t width, const size_t height, std::shared_ptr<ShaderProgram> shaderProgram)
	  : mShaderProgram(shaderProgram),
	    mTexture(nullptr),
		mVertexBuffer(nullptr)
{
	InitByColor(width, height, kDefaultColor, kDefaultColor, kDefaultColor, kDefaultColor);
}

Sprite::Sprite(const size_t width, const size_t height, const Math::Vector2f leftTopTexCoord,
			   const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
			   const Math::Vector2f rightBottomTexCoord, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram)
	  : mShaderProgram(shaderProgram),
	    mTexture(texture),
		mVertexBuffer(nullptr)
{
	InitByTexture(width, height, leftTopTexCoord, rightTopTexCoord, leftBottomTexCoord, rightBottomTexCoord);
}

Sprite::Sprite(const size_t width, const size_t height, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram)
	  : mShaderProgram(shaderProgram),
	    mTexture(texture),
		mVertexBuffer(nullptr)
{
	InitByTexture(width, height, kDefaultLeftTopTexCoord, kDefaultRightTopTexCoord,
				  kDefaultLeftBottomTexCoord, kDefaultRightBottomTexCoord);
}

std::shared_ptr<VertexBuffer> Sprite::GetVertexBuffer() const
{
	return mVertexBuffer;
}

std::shared_ptr<Texture2D> Sprite::GetTexture() const
{
	return mTexture;
}

std::shared_ptr<ShaderProgram> Sprite::GetShaderProgram() const
{
	return mShaderProgram;
}

typename Sprite::BaseData Sprite::BuildBaseData(const size_t spriteWidth, const size_t spriteHeight)
{
	VertexPositionArray positions = VertexPositionArray();
	IndexArray indicies = IndexArray();

	//
	// First triangle				 |\
	// 	 	 	 	 	 	 	 |	 | \    ^
	// 	 	 	 	 	 	 	 v	 |  \   |
	// 	 	 	 	 	 	 	 	 |___\
	//
	//								   ->


	// left top
	positions[0] = Math::Vector2f(0.0f, 0.0f);
	indicies[0] = 0;

	// left bottom
	positions[1] = Math::Vector2f(0.0f, static_cast<float>(spriteHeight));
	indicies[1] = 1;

	// right bottom
	positions[2] = Math::Vector2f(static_cast<float>(spriteWidth), static_cast<float>(spriteHeight));
	indicies[2] = 2;

	//								____
	// Second triangle				\ 	|
	// 	 	 	 	 	 	 	 	 \ 	|
	// 	 	 	 	 	 	 	 	  \	|
	// 	 	 	 	 	 	 	 	   \|


	// left top
	indicies[3] = 0;

	// right bottom
	indicies[4] = 2;

	// right top
	positions[3] = Math::Vector2f(static_cast<float>(spriteWidth), 0.0f);
	indicies[5] = 3;

	return std::make_pair(positions, indicies);
}

void Sprite::InitByColor(const size_t width, const size_t height, const Color& leftTop, const Color& rightTop,
		   	   	  	  	 const Color& leftBottom, const Color& rightBottom)
{
	std::array<ColorVertex, kSpriteVertexCount> vertices;
	BaseData baseData = BuildBaseData(width, height);

	VertexPositionArray positions = std::get<0>(baseData);
	IndexArray indices = std::get<1>(baseData);

	for (size_t i = 0; i < kSpriteVertexCount; i++)
	{
		vertices[i].x = positions[i].GetX();
		vertices[i].y = positions[i].GetY();
	}

	FillColor(vertices[0], leftTop);
	FillColor(vertices[1], leftBottom);
	FillColor(vertices[2], rightBottom);
	FillColor(vertices[3], rightTop);

	mVertexBuffer = std::make_shared<VertexBuffer>(vertices.data(), indices.data(), kSpriteVertexCount, kSpriteIndexCount, BufferUsage::Static);
}

void Sprite::InitByTexture(const size_t width, const size_t height, const Math::Vector2f leftTopTexCoord,
		   	   	    const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
		   	   	    const Math::Vector2f rightBottomTexCoord)
{
	std::array<TextureVertex, kSpriteVertexCount> vertices;
	BaseData baseData = BuildBaseData(width, height);

	VertexPositionArray positions = std::get<0>(baseData);
	IndexArray indices = std::get<1>(baseData);

	for (size_t i = 0; i < kSpriteVertexCount; i++)
	{
		vertices[i].x = positions[i].GetX();
		vertices[i].y = positions[i].GetY();
	}

	FillTexCoord(vertices[0], leftTopTexCoord);
	FillTexCoord(vertices[1], leftBottomTexCoord);
	FillTexCoord(vertices[2], rightBottomTexCoord);
	FillTexCoord(vertices[3], rightTopTexCoord);

	mVertexBuffer = std::make_shared<VertexBuffer>(vertices.data(), indices.data(), kSpriteVertexCount, kSpriteIndexCount, BufferUsage::Static);
}

} // Pacman namespace
