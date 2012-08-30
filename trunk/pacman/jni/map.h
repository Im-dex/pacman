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

enum class MapCellType : uint8_t
{
	Empty = 0,
	Wall  = 1,
	Door  = 2
};

static const size_t kCellTypesCount = 3;

class Map
{
public:

	Map(const std::string& textData);
	Map(const Map&) = delete;
	~Map() = default;

	Map& operator= (const Map&) = delete;

	void AttachToScene(SceneManager& sceneManager);

	size_t GetCellSize() const
	{
		return mCellSize;
	}

    // return position of the cell center
    Math::Vector2s GetCellPosition(const size_t rowIndex, const size_t columnIndex);

    Math::Vector2s GetCellPosition(const Math::Vector2s& cellIndices);

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
		MapCellType   leftTop;
		MapCellType   rightTop;
		MapCellType   leftBottom;
		MapCellType   rightBottom;
	};

    void ParseJsonData(const std::string& data);

	std::shared_ptr<Sprite> GenerateSprite();

	std::shared_ptr<Texture2D> GenerateTexture(TextureRegion* textureRegion);

	void CleanArtifacts(byte_t* buffer, const size_t textureWidth);

    MapCellType GetCell(const size_t rowIndex, const size_t columnIndex) const;

	MapCellType GetCell(const Math::Vector2s& cellIndices) const;

	NeighborsInfo GetDirectNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const;
	FullNeighborsInfo GetFullNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const;

	std::vector<MapCellType> mCells;
    Rect<size_t>             mRect;
	uint8_t 				 mRowsCount;
	uint8_t 				 mColumnsCount;
	uint8_t					 mCellSize;
    uint8_t                  mCellSizeHalf;
	uint8_t					 mCellSizeQuarter;

	std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace
