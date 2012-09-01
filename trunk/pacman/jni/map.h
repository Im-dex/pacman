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

enum class DotType : uint8_t
{
    None  = 0,
    Small = 3,
    Big   = 4
};

static const size_t kDotTypesCount = 2;

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

typedef Math::Vector2<uint16_t> CellIndex;

class Map
{
public:

	Map(const std::string& textData);
	Map(const Map&) = delete;
	~Map() = default;

	Map& operator= (const Map&) = delete;

	void AttachToScene(SceneManager& sceneManager);

    MapCellType GetCell(const uint16_t rowIndex, const uint16_t columnIndex) const;

    MapCellType GetCell(const CellIndex& index) const;

    SpritePosition GetCellCenterPos(const uint16_t rowIndex, const uint16_t columnIndex) const;

    SpritePosition GetCellCenterPos(const CellIndex& index) const;

    MapNeighborsInfo GetDirectNeighbors(const uint16_t rowIndex, const uint16_t columnIndex) const;

    MapNeighborsInfo GetDirectNeighbors(const CellIndex& index) const;

    FullMapNeighborsInfo GetFullNeighbors(const uint16_t rowIndex, const uint16_t columnIndex) const;

    FullMapNeighborsInfo GetFullNeighbors(const CellIndex& index) const;

	uint16_t GetCellSize() const
	{
		return mCellSize;
	}

    uint16_t GetRowsCont() const
    {
        return mRowsCount;
    }

    uint16_t GetColumnsCount() const
    {
        return mColumnsCount;
    }

private:

    void ParseJsonData(const std::string& data);

	std::shared_ptr<Sprite> GenerateSprite();

	std::shared_ptr<Texture2D> GenerateTexture(TextureRegion* textureRegion);

	void CleanArtifacts(byte_t* buffer, const size_t textureWidth);

	std::vector<MapCellType> mCells;
    std::vector<DotType>     mDots;
    SpriteRegion             mRect;
	uint16_t 				 mRowsCount;
	uint16_t 				 mColumnsCount;
	uint16_t				 mCellSize;
    uint16_t                 mCellSizeHalf;
	uint16_t				 mCellSizeQuarter;

	std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace
