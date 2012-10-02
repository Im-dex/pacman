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

	Map(const Size cellSize, const CellIndex::value_t rowsCount, const size_t viewportWidth,
        const size_t viewportHeight, const CellIndex& leftTunnelExit,
        const CellIndex& rightTunnelExit, const std::vector<MapCellType>& cells);

	Map(const Map&) = delete;
	~Map() = default;

	Map& operator= (const Map&) = delete;

	void AttachToScene(SceneManager& sceneManager);

    MapCellType GetCell(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const;

    MapCellType GetCell(const CellIndex& index) const;

    // return cell index that contains position point,
    // if position not in map region returns kCellIndexNPos
    CellIndex GetCellIndex(const Position& position) const;

    Position GetCellCenterPos(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const;

    Position GetCellCenterPos(const CellIndex& cell) const;

    // find cells where placed region
    CellIndexArray FindCells(const SpriteRegion& region) const;

    MapNeighborsInfo GetDirectNeighbors(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const;

    MapNeighborsInfo GetDirectNeighbors(const CellIndex& index) const;

    FullMapNeighborsInfo GetFullNeighbors(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const;

    FullMapNeighborsInfo GetFullNeighbors(const CellIndex& index) const;

	Size GetCellSize() const
	{
		return mCellSize;
	}

    CellIndex::value_t GetRowsCount() const
    {
        return mRowsCount;
    }

    CellIndex::value_t GetColumnsCount() const
    {
        return mColumnsCount;
    }

    CellIndex GetLeftTunnelExit() const
    {
        return mLeftTunnelExit;
    }

    CellIndex GetRightTunnelExit() const
    {
        return mRightTunnelExit;
    }

private:

	std::shared_ptr<Sprite> GenerateSprite();

	std::shared_ptr<Texture2D> GenerateTexture(TextureRegion* textureRegion);

	void CleanArtifacts(byte_t* buffer, const size_t textureWidth);

    const Size			     mCellSize;
    const Size               mCellSizeHalf;
    const Size			     mCellSizeQuarter;
    const CellIndex::value_t mRowsCount;
    const CellIndex::value_t mColumnsCount;
    const CellIndex          mLeftTunnelExit;
    const CellIndex          mRightTunnelExit;
	std::vector<MapCellType> mCells;
    SpriteRegion             mRect;

	std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace
