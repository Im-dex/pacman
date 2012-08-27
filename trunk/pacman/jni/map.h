#pragma once

#include "base.h"
#include "math/vector2.h"
#include "sprite.h"

#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace Pacman {

class SceneNode;
class Texture2D;
class SceneManager;

static const size_t kCellTypesCount = 4;

enum class MapCellType : uint8_t
{
	Empty = 0,
	Wall  = 1,
	Door  = 2,
	Space = 3 // out of map area
};

class Map
{
public:

	Map(const std::string& textData);
	Map(const Map&) = delete;
	~Map() = default;

	Map& operator= (const Map&) = delete;

	void AttachToScene(SceneManager& sceneManager);

private:

	struct NeighborsInfo
	{
		MapCellType left;
		MapCellType right;
		MapCellType top;
		MapCellType bottom;
	};

	struct FullNeighborsInfo
	{
		NeighborsInfo directInfo;
		MapCellType	  leftTop;
		MapCellType   rightTop;
		MapCellType   leftBottom;
		MapCellType   rightBottom;
	};

	std::shared_ptr<Sprite> GenerateSprite();

	std::shared_ptr<Texture2D> GenerateTexture(const size_t textureWidth, const size_t textureHeight,
						 	 	 	 		   const size_t mapWidth, const size_t mapHeight, TextureRegion* textureRegion);

	void CleanArtifacts(byte_t* buffer, const size_t textureWidth);

	MapCellType GetCell(const uint8_t rowIndex, const uint8_t columnIndex) const;

	NeighborsInfo GetDirectNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const;
	FullNeighborsInfo GetFullNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const;

	std::vector<MapCellType> mCells;
	uint8_t 				 mRowsCount;
	uint8_t 				 mColumnsCount;
	uint8_t					 mCellSize;
	uint8_t					 mCellQuarter;

	std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace
