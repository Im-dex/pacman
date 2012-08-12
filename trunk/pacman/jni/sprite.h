#pragma once

#include "drawable.h"
#include "color.h"
#include "math/vector2.h"

#include <array>
#include <memory>
#include <tuple>

namespace Pacman {

class Texture2D;
class ShaderProgram;
class VertexBuffer;
struct Vertex;

class Sprite : public IDrawable
{
public:

	Sprite() = delete;

	Sprite(const size_t width, const size_t height, const Color& leftTop, const Color& rightTop,
		   const Color& leftBottom, const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const size_t width, const size_t height, std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const size_t width, const size_t height, const Math::Vector2f leftTopTexCoord,
		   const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
		   const Math::Vector2f rightBottomTexCoord, std::shared_ptr<Texture2D> texture,
		   std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const size_t width, const size_t height, std::shared_ptr<Texture2D> texture,
		   std::shared_ptr<ShaderProgram> shaderProgram);

	Sprite(const Sprite&) = default;
	~Sprite() = default;

	Sprite& operator= (const Sprite&) = default;

	virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() const;

	virtual std::shared_ptr<Texture2D> GetTexture() const;

	virtual std::shared_ptr<ShaderProgram> GetShaderProgram() const;

private:

	enum { kSpriteVertexCount = 4 };
	enum { kSpriteIndexCount = 6 };
	typedef std::array<Math::Vector2f, kSpriteVertexCount> VertexPositionArray;
	typedef std::array<uint16_t, kSpriteIndexCount> IndexArray;
	typedef std::tuple<VertexPositionArray, IndexArray> BaseData;

	BaseData BuildBaseData(const size_t spriteWidth, const size_t spriteHeight);

	void InitByColor(const size_t width, const size_t height, const Color& leftTop, const Color& rightTop,
			   	   	 const Color& leftBottom, const Color& rightBottom);

	void InitByTexture(const size_t width, const size_t height, const Math::Vector2f leftTopTexCoord,
			   	   	   const Math::Vector2f rightTopTexCoord, const Math::Vector2f leftBottomTexCoord,
			   	   	   const Math::Vector2f rightBottomTexCoord);

	std::shared_ptr<VertexBuffer> mVertexBuffer;
	std::shared_ptr<Texture2D> mTexture;
	std::shared_ptr<ShaderProgram> mShaderProgram;
};

} // Pacman namespace
