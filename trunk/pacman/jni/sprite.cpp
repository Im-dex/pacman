#include "sprite.h"
#include "base.h"
#include "error.h"
#include "texture.h"
#include "vertex_buffer.h"

#include <tuple>

namespace Pacman {

static const size_t kSpriteVertexCount = 4;
static const size_t kSpriteIndexCount = 6;
static const Color kDefaultColor = Color::kGreen;
static const TextureRegion kDefaultRegion = TextureRegion(Math::Vector2f::kZero, 1.0f, 1.0f);

typedef std::array<Math::Vector2f, kSpriteVertexCount> VertexPositionArray;
typedef std::array<uint16_t, kSpriteIndexCount> IndexArray;
typedef std::tuple<VertexPositionArray, IndexArray> BaseData;

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

static BaseData BuildBaseData(const SpriteRegion& region)
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
	positions[0] = Math::Vector2f(static_cast<float>(region.GetPosX()), static_cast<float>(region.GetPosY()));
	indicies[0] = 0;

	// left bottom
	positions[1] = Math::Vector2f(static_cast<float>(region.GetLeftBottomPosX()), static_cast<float>(region.GetLeftBottomPosY()));
	indicies[1] = 1;

	// right bottom
	positions[2] = Math::Vector2f(static_cast<float>(region.GetRightBottomPosX()), static_cast<float>(region.GetRightBottomPosY()));
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
	positions[3] = Math::Vector2f(static_cast<float>(region.GetRightTopPosX()), static_cast<float>(region.GetRightTopPosY()));
	indicies[5] = 3;

	return std::make_pair(positions, indicies);
}

//===========================================================================================================

Sprite::Sprite(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, const Color& leftBottom,
			   const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mShaderProgram(shaderProgram),
	    mTexture(nullptr),
		mVertexBuffer(nullptr),
		mAlphaBlend(alphaBlend)
{
	InitByColor(region, leftTop, rightTop, leftBottom, rightBottom);
}

Sprite::Sprite(const SpriteRegion& region, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mShaderProgram(shaderProgram),
	    mTexture(nullptr),
		mVertexBuffer(nullptr),
		mAlphaBlend(alphaBlend)
{
	InitByColor(region, kDefaultColor, kDefaultColor, kDefaultColor, kDefaultColor);
}

Sprite::Sprite(const SpriteRegion& region, const TextureRegion& textureRegion, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mShaderProgram(shaderProgram),
	    mTexture(texture),
		mVertexBuffer(nullptr),
		mAlphaBlend(alphaBlend)
{
	InitByTexture(region, textureRegion);
}

Sprite::Sprite(const SpriteRegion& region, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend)
	  : mShaderProgram(shaderProgram),
	    mTexture(texture),
		mVertexBuffer(nullptr),
		mAlphaBlend(alphaBlend)
{
	InitByTexture(region, kDefaultRegion);
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

bool Sprite::HasAlphaBlend() const
{
	return mAlphaBlend;
}

void Sprite::InitByColor(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, 
						 const Color& leftBottom, const Color& rightBottom)
{
	std::array<ColorVertex, kSpriteVertexCount> vertices;
	BaseData baseData = BuildBaseData(region);

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

void Sprite::InitByTexture(const SpriteRegion& region, const TextureRegion& textureRegion)
{
	std::array<TextureVertex, kSpriteVertexCount> vertices;
	BaseData baseData = BuildBaseData(region);

	VertexPositionArray positions = std::get<0>(baseData);
	IndexArray indices = std::get<1>(baseData);

	for (size_t i = 0; i < kSpriteVertexCount; i++)
	{
		vertices[i].x = positions[i].GetX();
		vertices[i].y = positions[i].GetY();
	}

	FillTexCoord(vertices[0], textureRegion.GetPosition());
	FillTexCoord(vertices[1], Math::Vector2f(textureRegion.GetPosX(), textureRegion.GetPosY() + textureRegion.GetHeight()));
	FillTexCoord(vertices[2], Math::Vector2f(textureRegion.GetPosX() + textureRegion.GetWidth(), textureRegion.GetPosY() + textureRegion.GetHeight()));
	FillTexCoord(vertices[3], Math::Vector2f(textureRegion.GetPosX() + textureRegion.GetWidth(), textureRegion.GetPosY()));

	mVertexBuffer = std::make_shared<VertexBuffer>(vertices.data(), indices.data(), kSpriteVertexCount, kSpriteIndexCount, BufferUsage::Static);
}

} // Pacman namespace
