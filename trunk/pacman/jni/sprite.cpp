#include "sprite.h"
#include "base.h"
#include "texture.h"
#include "vertex_buffer.h"

namespace Pacman {

FORCEINLINE void FillColor(Vertex& vertex, const Color& color)
{
	vertex.r = color.GetRedFloat();
	vertex.g = color.GetGreenFloat();
	vertex.b = color.GetBlueFloat();
	vertex.a = color.GetAlphaFloat();
}

Sprite::Sprite(const float width, const float height, const Color& leftTop, const Color& rightTop,
			   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram)
	  : Drawable(std::shared_ptr<VertexBuffer>(), std::shared_ptr<Texture2D>(), shaderProgram)
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

/*Sprite::Sprite(const float width, const float height, std::shared_ptr<Texture2D> texture,
			   std::shared_ptr<ShaderProgram> shaderProgram)
{

}*/

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

} // Pacman namespace
