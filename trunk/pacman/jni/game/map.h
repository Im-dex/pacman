#pragma once

#include <vector>
#include <memory>
#include <string>

#include "sprite.h"
#include "game_typedefs.h"

namespace Pacman {

class SceneNode;
class Texture2D;
class SceneManager;

enum class MapCellType : uint8_t
{
	Empty = 0,
	Wall  = 1,
	Door  = 2,
    Space = 3 // out of map area
};

static const size_t kCellTypesCount = 4;

struct MapNeighborsInfo
{
    MapCellType left;
    MapCellType right;
    MapCellType top;
    MapCellType bottom;
};

struct FullMapNeighborsInfo
{
    MapNeighborsInfo directInfo;
    MapCellType      leftTop;
    MapCellType      rightTop;
    MapCellType      leftBottom;
    MapCellType      rightBottom;
};

class Map
{
public:

	Map(const uint16_t cellSize, const uint16_t rowsCount, const size_t viewportWidth,
        const size_t viewportHeight, const std::vector<MapCellType>& cells);

	Map(const Map&) = delete;
	~Map() = default;

	Map& operator= (const Map&) = delete;

	void AttachToScene(SceneManager& sceneManager);

    MapCellType GetCell(const uint16_t rowIndex, const uint16_t columnIndex) const;

    MapCellType GetCell(const CellIndex& index) const;

    SpritePosition GetCellCenterPos(const uint16_t rowIndex, const uint16_t columnIndex) const;

    SpritePosition GetCellCenterPos(const CellIndex& index) const;

    // find cell where placed region
    CellIndex FindCell(const SpriteRegion& region) const;

    MapNeighborsInfo GetDirectNeighbors(const uint16_t rowIndex, const uint16_t columnIndex) const;

    MapNeighborsInfo GetDirectNeighbors(const CellIndex& index) const;

    FullMapNeighborsInfo GetFullNeighbors(const uint16_t rowIndex, const uint16_t columnIndex) const;

    FullMapNeighborsInfo GetFullNeighbors(const CellIndex& index) const;

	uint16_t GetCellSize() const
	{
		return mCellSize;
	}

    uint16_t GetRowsCount() const
    {
        return mRowsCount;
    }

    uint16_t GetColumnsCount() const
    {
        return mColumnsCount;
    }

private:

	std::shared_ptr<Sprite> GenerateSprite();

	std::shared_ptr<Texture2D> GenerateTexture(TextureRegion* textureRegion);

	void CleanArtifacts(byte_t* buffer, const size_t textureWidth);

	std::vector<MapCellType> mCells;
    SpriteRegion             mRect;
	uint16_t 				 mRowsCount;
	uint16_t 				 mColumnsCount;
	uint16_t				 mCellSize;
    uint16_t                 mCellSizeHalf;
	uint16_t				 mCellSizeQuarter;

	std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace
