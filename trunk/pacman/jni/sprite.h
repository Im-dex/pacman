#pragma once

#include "drawable.h"
#include "color.h"

#include <memory>

namespace Pacman {

class Texture2D;
class ShaderProgram;
class VertexBuffer;

class Sprite : public Drawable
{
public:

	Sprite() = delete;
	Sprite(const float width, const float height, const Color& leftTop, const Color& rightTop,
		   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram);
	//Sprite(const float width, const float height, std::shared_ptr<Texture2D> texture,
		  // std::shared_ptr<ShaderProgram> shaderProgram);
	Sprite(const Sprite&) = default;
	~Sprite() = default;

	Sprite& operator= (const Sprite&) = default;

private:

	std::shared_ptr<VertexBuffer> BuildVertexColorBuffer(const float width, const float height, const Color& leftTop, const Color& rightTop,
			   	   	   	   	   	   	   	   	   	   	     const Color& leftBottom, const Color& rightBottom);
};

} // Pacman namespace
