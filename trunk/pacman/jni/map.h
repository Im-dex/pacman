#pragma once

#include "base.h"
#include "math/vector2.h"
#include "sprite.h"

#include <vector>
#include <memory>
#include <cstdint>
#include <string>

namespace Pacman {

class SceneNode;
class Texture2D;

static const size_t kCellTypesCount = 3;

enum class MapCellType : uint8_t
{
	Empty = 0,
	Wall  = 1,
	Door  = 2
};

class Map
{
public:

	Map();
	Map(const Map&) = delete;
	~Map() = default;

	Map& operator= (const Map&) = delete;

	void Load(const std::string& textData, const size_t screenWidth, const size_t screenHeight);

private:

	Sprite GenerateSprite(const size_t screenWidth, const size_t screenHeight, Math::Vector2f* position);

	std::shared_ptr<Texture2D> GenerateTexture(const size_t textureWidth, const size_t textureHeight,
						 	 	 	 	 	   const size_t mapWidth, const size_t mapHeight,
						 	 	 	 	 	   Math::Vector2f* leftTopTexCoord, Math::Vector2f* rightTopTexCoord,
						 	 	 	 	 	   Math::Vector2f* leftBottomTexCoord, Math::Vector2f* rightBottomTexCoord);

	enum class ReadState
	{
		ReadCellWidth,
		ReadCellHeight,
		ReadRow,
		SkipLine
	};

	std::vector<MapCellType> mCells;
	uint8_t 				 mRowsCount;
	uint8_t 				 mColumnsCount;
	uint8_t					 mCellSize;

	std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace
