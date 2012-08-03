#include "sprite.h"
#include "base.h"
#include "texture.h"
#include "vertex_buffer.h"

namespace Pacman {

static const Color kDefaultColor = Color::kGreen;
static const Math::Vector2f kDefaultLeftTopTexCoord = Math::Vector2f(0.0f, 0.0f);
static const Math::Vector2f kDefaultRightTopTexCoord = Math::Vector2f(1.0f, 0.0f);
static const Math::Vector2f kDefaultLeftBottomTexCoord = Math::Vector2f(0.0f, 1.0f);
static const Math::Vector2f kDefaultRightBottomTexCoord = Math::Vector2f(1.0f, 1.0f);

FORCEINLINE void FillColor(Vertex& vertex, const Color& color)
{
	vertex.r = color.GetRedFloat();
	vertex.g = color.GetGreenFloat();
	vertex.b = color.GetBlueFloat();
	vertex.a = color.GetAlphaFloat();
}

FORCEINLINE void FillTexCoord(Vertex& vertex, const Math::Vector2f texCoord)
{
	vertex.u = texCoord.GetX();
	vertex.v = texCoord.GetY();
}

//===========================================================================================================

Sprite::Sprite(const float width, const float height, const Color& leftTop, const Color& rightTop,
			   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram)
	  : Drawable(nullptr, nullptr, shaderProgram)
{
	InitByColor(width, height, leftTop, rightTop, leftBottom, rightBottom);
}

Sprite::Sprite(const float width, const float height, std::shared_ptr<ShaderProgram> shaderProgram)
	  : Drawable(nullptr, nullptr, shaderProgram)
{
	InitByColor(width, height, kDefaultColor, kDefaultColor, kDefaultColor, kDefaultColor);
}

Sprite::Sprite(const float width, const float height, const Math::Vector2f leftTopTexCoord,
			   const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
			   const Math::Vector2f rightBottomTexCoord, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram)
	  : Drawable(nullptr, texture, shaderProgram)
{
	InitByTexture(width, height, leftTopTexCoord, rightTopTexCoord, leftBottomTexCoord, rightBottomTexCoord);
}

Sprite::Sprite(const float width, const float height, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram)
	  : Drawable(nullptr, texture, shaderProgram)
{
	InitByTexture(width, height, kDefaultLeftTopTexCoord, kDefaultRightTopTexCoord,
				  kDefaultLeftBottomTexCoord, kDefaultRightBottomTexCoord);
}

Sprite::SpriteVertexArray Sprite::BuildVertexBuffer(const float spriteWidth, const float spriteHeight)
{
	SpriteVertexArray vertices = SpriteVertexArray();

	//
	// First triangle				 |\
	// 	 	 	 	 	 	 	 	 | \
	// 	 	 	 	 	 	 	 	 |  \
	// 	 	 	 	 	 	 	 	 |___\


	// left top
	vertices[0].x = 0.0f;
	vertices[0].y = 0.0f;

	// left bottom
	vertices[1].x = 0.0f;
	vertices[1].y = spriteHeight;

	// right bottom
	vertices[2].x = spriteWidth;
	vertices[2].y = spriteHeight;

	//								____
	// Second triangle				\ 	|
	// 	 	 	 	 	 	 	 	 \ 	|
	// 	 	 	 	 	 	 	 	  \	|
	// 	 	 	 	 	 	 	 	   \|


	// left top
	vertices[3].x = 0.0f;
	vertices[3].y = 0.0f;

	// right bottom
	vertices[4].x = spriteWidth;
	vertices[4].y = spriteHeight;

	// right top
	vertices[5].x = spriteWidth;
	vertices[5].y = 0.0f;

	return vertices;
}

void Sprite::InitByColor(const float width, const float height, const Color& leftTop, const Color& rightTop,
		   	   	  	  	 const Color& leftBottom, const Color& rightBottom)
{
	SpriteVertexArray vertices = BuildVertexBuffer(width, height);

	FillColor(vertices[0], leftTop);
	FillColor(vertices[1], leftBottom);
	FillColor(vertices[2], rightBottom);
	FillColor(vertices[3], leftTop);
	FillColor(vertices[4], rightBottom);
	FillColor(vertices[5], rightTop);

	const fvf_t fvf = static_cast<fvf_t>(FVFElement::Position) | static_cast<fvf_t>(FVFElement::Color);
	auto vertexBuffer = std::make_shared<VertexBuffer>(vertices.data(), kSpriteVerticesCount, fvf);
	SetVertexBuffer(vertexBuffer);
}

void Sprite::InitByTexture(const float width, const float height, const Math::Vector2f leftTopTexCoord,
		   	   	    const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
		   	   	    const Math::Vector2f rightBottomTexCoord)
{
	SpriteVertexArray vertices = BuildVertexBuffer(width, height);

	FillTexCoord(vertices[0], leftTopTexCoord);
	FillTexCoord(vertices[1], leftBottomTexCoord);
	FillTexCoord(vertices[2], rightBottomTexCoord);
	FillTexCoord(vertices[3], leftTopTexCoord);
	FillTexCoord(vertices[4], rightBottomTexCoord);
	FillTexCoord(vertices[5], rightTopTexCoord);

	const fvf_t fvf = static_cast<fvf_t>(FVFElement::Position) | static_cast<fvf_t>(FVFElement::TexCoords);
	auto vertexBuffer = std::make_shared<VertexBuffer>(vertices.data(), kSpriteVerticesCount, fvf);
	SetVertexBuffer(vertexBuffer);
}

} // Pacman namespace
