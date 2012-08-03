#pragma once

#include "drawable.h"
#include "color.h"
#include "math/vector2.h"

#include <array>
#include <memory>

namespace Pacman {

class Texture2D;
class ShaderProgram;
class VertexBuffer;
struct Vertex;

class Sprite : public Drawable
{
public:

	Sprite() = delete;

	Sprite(const float width, const float height, const Color& leftTop, const Color& rightTop,
		   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const float width, const float height, std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const float width, const float height, const Math::Vector2f leftTopTexCoord,
		   const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
		   const Math::Vector2f rightBottomTexCoord, std::shared_ptr<Texture2D> texture,
		   std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const float width, const float height, std::shared_ptr<Texture2D> texture,
		   std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const Sprite&) = default;
	~Sprite() = default;

	Sprite& operator= (const Sprite&) = default;

private:

	enum { kSpriteVerticesCount = 6 };
	typedef std::array<Vertex, kSpriteVerticesCount> SpriteVertexArray;

	 SpriteVertexArray BuildVertexBuffer(const float spriteWidth, const float spriteHeight);

	 void InitByColor(const float width, const float height, const Color& leftTop, const Color& rightTop,
			   	   	  const Color& leftBottom, const Color& rightBottom);

	 void InitByTexture(const float width, const float height, const Math::Vector2f leftTopTexCoord,
			   	   	    const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
			   	   	    const Math::Vector2f rightBottomTexCoord);
};

} // Pacman namespace
