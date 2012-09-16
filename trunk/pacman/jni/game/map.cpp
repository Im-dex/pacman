#include "map.h"

#include <cassert>
#include <complex>
#include <algorithm>

#include "log.h"
#include "utils.h"
#include "error.h"
#include "color.h"
#include "scene_node.h"
#include "engine.h"
#include "asset_manager.h"
#include "shader_program.h"
#include "texture.h"
#include "scene_manager.h"
#include "rect.h"

namespace Pacman {

static const Color kEmptyColor = Color::kBlack;
static const Color kWallColor = Color::kBlue;
static const Color kDoorColor = Color::kWhite;
static const Color kAlignColor = Color::kRed;

static const size_t kColorComponentsCount = 3; // RGB

static FORCEINLINE Color GetColor(const MapCellType type)
{
	switch (type)
	{
	case MapCellType::Empty:
		return kEmptyColor;
	case MapCellType::Wall:
		return kWallColor;
	case MapCellType::Door:
		return kDoorColor;
	}
}

// Fill region by color, all sizes is given in pixels (not in bytes!!!)
static void FillRegion(byte_t* colorBuffer, const uint16_t rowWidth, const SpriteRegion region, const Color color)
{
	const size_t bytesInRow = rowWidth * kColorComponentsCount;
	const size_t bytesInRegionRow = region.GetWidth() * kColorComponentsCount;

	std::unique_ptr<byte_t[]> row(new byte_t[bytesInRegionRow]);
	for (size_t i = 0; i < bytesInRegionRow; i += kColorComponentsCount)
	{
		row.get()[i]   = color.GetRed();
		row.get()[i+1] = color.GetGreen();
		row.get()[i+2] = color.GetBlue();
	}

	for (size_t i = 0; i < region.GetHeight(); i++)
	{
		const size_t bufferOffset = ((i + region.GetPosY()) * bytesInRow) + (region.GetPosX() * kColorComponentsCount);
		memcpy(colorBuffer + bufferOffset, row.get(), bytesInRegionRow);
	}
}

// cut cutSize pixels from left
static FORCEINLINE void CutLeft(SpriteRegion& region, const uint16_t cutSize)
{
	region.SetPosX(region.GetPosX() + cutSize);
	region.SetWidth(region.GetWidth() - cutSize);	
}

// cut cutSize pixels from right
static FORCEINLINE void CutRight(SpriteRegion& region, const uint16_t cutSize)
{
	region.SetWidth(region.GetWidth() - cutSize);
}

// cut cutSize pixels from top
static FORCEINLINE void CutTop(SpriteRegion& region, const uint16_t cutSize)
{
	region.SetPosY(region.GetPosY() + cutSize);
	region.SetHeight(region.GetHeight() - cutSize);
}

// cut cutSize pixels from bottom
static FORCEINLINE void CutBottom(SpriteRegion& region, const uint16_t cutSize)
{
	region.SetHeight(region.GetHeight() - cutSize);
}

//============================================================================================================================================

Map::Map(const uint16_t cellSize, const uint16_t rowsCount, const size_t viewportWidth,
         const size_t viewportHeight, const CellIndex& leftTunnelExit,
         const CellIndex& rightTunnelExit, const std::vector<MapCellType>& cells)
   : mCellSize(cellSize),
     mCellSizeHalf(cellSize/2),
     mCellSizeQuarter(cellSize/4),
     mRowsCount(rowsCount),
     mLeftTunnelExit(leftTunnelExit),
     mRightTunnelExit(rightTunnelExit),
     mColumnsCount(cells.size() / rowsCount),
     mCells(cells),
     mRect(0, 0, 0, 0)
{
    const uint16_t mapWidth = mColumnsCount * mCellSize;
    const uint16_t mapHeight = mRowsCount * mCellSize;

    const uint16_t leftRightPadding = (viewportWidth - mapWidth) / 2;
    const uint16_t topBottomPadding = (viewportHeight - mapHeight) / 2;

    mRect = SpriteRegion(leftRightPadding, topBottomPadding, mapWidth, mapHeight);
    LOGI("mRect: posx: %u, posy: %u", mRect.GetPosX(), mRect.GetPosY());
    std::shared_ptr<Sprite> sprite = GenerateSprite();
	mNode = std::make_shared<SceneNode>(sprite, SpritePosition::kZero, SpritePosition::kZero, Rotation::kZero);
}

void Map::AttachToScene(SceneManager& sceneManager)
{
	sceneManager.AttachNode(mNode);
}

MapCellType Map::GetCell(const uint16_t rowIndex, const uint16_t columnIndex) const
{
    PACMAN_CHECK_ERROR2((rowIndex <= mRowsCount) && (columnIndex <= mColumnsCount), ErrorCode::BadArgument,
                        MakeString("rowIndex: ", rowIndex, ", columnIndex: ", columnIndex).c_str());
    return mCells[(rowIndex * mColumnsCount) + columnIndex];
}

MapCellType Map::GetCell(const CellIndex& index) const
{
    return GetCell(index.GetX(), index.GetY());
}

SpritePosition Map::GetCellCenterPos(const uint16_t rowIndex, const uint16_t columnIndex) const
{
    return mRect.GetPosition() + (SpritePosition(columnIndex * mCellSize, rowIndex * mCellSize) + mCellSizeHalf);
}

SpritePosition Map::GetCellCenterPos(const CellIndex& index) const
{
    return GetCellCenterPos(index.GetX(), index.GetY());   
}

// TODO: optimize by quad-tree scene split
CellIndex Map::FindCell(const SpriteRegion& region) const
{
    for (size_t i = 0; i < mRowsCount; i++)
        for (size_t j = 0; j < mColumnsCount; j++)
        {
            if (region.HasPoint(GetCellCenterPos(i, j)))
                return CellIndex(i, j);
        }
}

MapNeighborsInfo Map::GetDirectNeighbors(const uint16_t rowIndex, const uint16_t columnIndex) const
{
    MapNeighborsInfo info;

    // left
    if (columnIndex == 0)
        info.left = MapCellType::Space;
    else
        info.left = GetCell(rowIndex, columnIndex - 1);

    // right
    if (columnIndex >= (mColumnsCount - 1))
        info.right = MapCellType::Space;
    else
        info.right = GetCell(rowIndex, columnIndex + 1);

    // top
    if (rowIndex == 0)
        info.top = MapCellType::Space;
    else
        info.top = GetCell(rowIndex - 1, columnIndex);

    // bottom
    if (rowIndex >= (mRowsCount - 1))
        info.bottom = MapCellType::Space;
    else
        info.bottom = GetCell(rowIndex + 1, columnIndex);

    return info;
}

MapNeighborsInfo Map::GetDirectNeighbors(const CellIndex& index) const
{
    return GetDirectNeighbors(index.GetX(), index.GetY());
}

FullMapNeighborsInfo Map::GetFullNeighbors(const uint16_t rowIndex, const uint16_t columnIndex) const
{
    FullMapNeighborsInfo info;
    info.directInfo = GetDirectNeighbors(rowIndex, columnIndex);

    // left top
    if ((columnIndex == 0) || (rowIndex == 0))
        info.leftTop = MapCellType::Space;
    else
        info.leftTop = GetCell(rowIndex - 1, columnIndex - 1);

    // right top
    if ((columnIndex >= (mColumnsCount - 1)) || (rowIndex == 0))
        info.rightTop = MapCellType::Space;
    else
        info.rightTop = GetCell(rowIndex - 1, columnIndex + 1);

    // left bottom
    if ((columnIndex == 0) || (rowIndex >= (mRowsCount - 1)))
        info.leftBottom = MapCellType::Space;
    else
        info.leftBottom = GetCell(rowIndex + 1, columnIndex - 1);

    // right bottom
    if ((columnIndex >= (mColumnsCount - 1)) || (rowIndex >= (mRowsCount - 1)))
        info.rightBottom = MapCellType::Space;
    else
        info.rightBottom = GetCell(rowIndex + 1, columnIndex + 1);

    return info;
}

FullMapNeighborsInfo Map::GetFullNeighbors(const CellIndex& index) const
{
    return GetFullNeighbors(index.GetX(), index.GetY());
}

std::shared_ptr<Sprite> Map::GenerateSprite()
{
	// lets generate!
	TextureRegion textureRegion(Math::Vector2f::kZero, 0.0f, 0.0f);
	std::shared_ptr<Texture2D> texture = GenerateTexture(&textureRegion);

	AssetManager& assetManager = GetEngine()->GetAssetManager();
	std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(AssetManager::kDefaultTextureVertexShader, AssetManager::kDefaultTextureFragmentShader);

	return std::make_shared<Sprite>(mRect, textureRegion, texture, shaderProgram, false);
}

std::shared_ptr<Texture2D> Map::GenerateTexture(TextureRegion* textureRegion)
{
    // expand to POT --> TODO: check that device supports this texture size  <--
    const size_t textureWidth = NextPOT(mRect.GetWidth());
    const size_t textureHeight = NextPOT(mRect.GetHeight());

	const size_t bufferSize = textureWidth * textureHeight * kColorComponentsCount;
	const size_t bytesInRow = textureWidth * kColorComponentsCount;
	std::unique_ptr<byte_t[]> buffer(new byte_t[bufferSize]);

	//
	// fill map
	//
	for (size_t i = 0; i < mRowsCount; i++)
	{
		for (size_t j = 0; j < mColumnsCount; j++)
		{
			MapCellType cell = GetCell(i, j);
			SpriteRegion cellRegion(j * mCellSize, i * mCellSize, mCellSize, mCellSize);

			if (cell == MapCellType::Door)
			{
				// cut the ghost house door height
				CutTop(cellRegion, mCellSizeQuarter);
				CutBottom(cellRegion, mCellSizeQuarter);
			} 
			else if (cell == MapCellType::Wall)
			{
				MapNeighborsInfo neighbors = GetDirectNeighbors(i, j);
				if ((neighbors.left == MapCellType::Empty) || (neighbors.left == MapCellType::Space))
					CutLeft(cellRegion, mCellSizeQuarter); // cut left side
				if ((neighbors.right == MapCellType::Empty) || (neighbors.right == MapCellType::Space))
					CutRight(cellRegion, mCellSizeQuarter); // cut right side
				if ((neighbors.top == MapCellType::Empty) || (neighbors.top == MapCellType::Space))
					CutTop(cellRegion, mCellSizeQuarter); // cut top side
				if ((neighbors.bottom == MapCellType::Empty) || (neighbors.bottom == MapCellType::Space))
					CutBottom(cellRegion, mCellSizeQuarter); // cut bottom side
			}

			FillRegion(buffer.get(), textureWidth, cellRegion, GetColor(cell));
		}
	}

	CleanArtifacts(buffer.get(), textureWidth);

	//
	// fill align range
	//
#ifdef PACMAN_DEBUG_MAP_TEXTURE
	// fill right align rectangle
	SpriteRegion rightRectangle(mRect.GetWidth(), 0, textureWidth - mRect.GetWidth(), mRect.GetHeight());
	FillRegion(buffer.get(), textureWidth, rightRectangle, kAlignColor);

	// fill bottom align rectangle
	SpriteRegion bottomRectangle(0, mRect.GetHeight(), textureWidth, textureHeight - mRect.GetHeight());
	FillRegion(buffer.get(), textureWidth, bottomRectangle, kAlignColor);

	//
	// calculate texcoords
	//
	const float u = static_cast<float>(mRect.GetWidth()) / static_cast<float>(textureWidth);
	const float v = static_cast<float>(mRect.GetHeight()) / static_cast<float>(textureHeight);
	*textureRegion = TextureRegion(Math::Vector2f::kZero, u, v);
#endif

	//
	// make texture
	//
	return std::make_shared<Texture2D>(textureWidth, textureHeight, buffer.get(), TextureFiltering::None,
									   TextureRepeat::None, PixelFormat::RGB_888);
}

//===============================================================================================================================================

void Map::CleanArtifacts(byte_t* buffer, const size_t textureWidth)
{
	// TRICK: (TODO: maybe need to find more simple way to do this)
	// draw small rectangle in the corner for hide an artifact
	for (size_t i = 0; i < mRowsCount; i++)
	{
		for (size_t j = 0; j < mColumnsCount; j++)
		{
			MapCellType cell = GetCell(i, j);
			if (cell != MapCellType::Wall)
				continue;

			SpriteRegion cellRegion(j * mCellSize, i * mCellSize, mCellSize, mCellSize);
			FullMapNeighborsInfo neighbors = GetFullNeighbors(i, j);

			if (neighbors.leftTop == MapCellType::Empty &&
				neighbors.directInfo.left == MapCellType::Wall &&
				neighbors.directInfo.top == MapCellType::Wall)
			{
				SpriteRegion artifactRegion(cellRegion.GetPosition(), mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.leftTop));
			}

			if (neighbors.rightTop == MapCellType::Empty &&
				neighbors.directInfo.right == MapCellType::Wall &&
				neighbors.directInfo.top == MapCellType::Wall)
			{
				SpritePosition pos = cellRegion.GetRightTopPos();
				pos.SetX(pos.GetX() - mCellSizeQuarter);
				SpriteRegion artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.rightTop));
			}

			if (neighbors.leftBottom == MapCellType::Empty &&
				neighbors.directInfo.left == MapCellType::Wall &&
				neighbors.directInfo.bottom == MapCellType::Wall)
			{
				SpritePosition pos = cellRegion.GetLeftBottomPos();
				pos.SetY(pos.GetY() - mCellSizeQuarter);
				SpriteRegion artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.leftBottom));
			}

			if (neighbors.rightBottom == MapCellType::Empty &&
				neighbors.directInfo.right == MapCellType::Wall &&
				neighbors.directInfo.bottom == MapCellType::Wall)
			{
				SpritePosition pos = cellRegion.GetRightBottomPos();
				pos.SetX(pos.GetX() - mCellSizeQuarter);
				pos.SetY(pos.GetY() - mCellSizeQuarter);
				SpriteRegion artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.rightBottom));
			}
		}
	}
}

} // Pacman namespace
