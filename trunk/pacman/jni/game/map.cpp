#include "map.h"

#include <cassert>
#include <complex>
#include <algorithm>
#include <cmath>

#include "log.h"
#include "utils.h"
#include "common.h"
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
static void FillRegion(byte_t* colorBuffer, const Size rowWidth, const SpriteRegion region, const Color color)
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
static FORCEINLINE void CutLeft(SpriteRegion& region, const Size cutSize)
{
	region.SetPosX(region.GetPosX() + cutSize);
	region.SetWidth(region.GetWidth() - cutSize);	
}

// cut cutSize pixels from right
static FORCEINLINE void CutRight(SpriteRegion& region, const Size cutSize)
{
	region.SetWidth(region.GetWidth() - cutSize);
}

// cut cutSize pixels from top
static FORCEINLINE void CutTop(SpriteRegion& region, const Size cutSize)
{
	region.SetPosY(region.GetPosY() + cutSize);
	region.SetHeight(region.GetHeight() - cutSize);
}

// cut cutSize pixels from bottom
static FORCEINLINE void CutBottom(SpriteRegion& region, const Size cutSize)
{
	region.SetHeight(region.GetHeight() - cutSize);
}

//============================================================================================================================================

Map::Map(const Size cellSize, const CellIndex::value_t rowsCount, const size_t viewportWidth,
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
    const Size mapWidth = mColumnsCount * mCellSize;
    const Size mapHeight = mRowsCount * mCellSize;

    const Size leftRightPadding = (viewportWidth - mapWidth) / 2;
    const Size topBottomPadding = (viewportHeight - mapHeight) / 2;

    mRect = SpriteRegion(leftRightPadding, topBottomPadding, mapWidth, mapHeight);
    LogI("mRect: posx: %u, posy: %u", mRect.GetPosX(), mRect.GetPosY());
    std::shared_ptr<Sprite> sprite = GenerateSprite();
	mNode = std::make_shared<SceneNode>(sprite, Position::kZero, Rotation::kZero);
}

void Map::AttachToScene(SceneManager& sceneManager)
{
	sceneManager.AttachNode(mNode);
}

MapCellType Map::GetCell(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const
{
    PACMAN_CHECK_ERROR2((rowIndex <= mRowsCount) && (columnIndex <= mColumnsCount), ErrorCode::BadArgument,
                        MakeString("rowIndex: ", rowIndex, ", columnIndex: ", columnIndex).c_str());
    return mCells[(rowIndex * mColumnsCount) + columnIndex];
}

MapCellType Map::GetCell(const CellIndex& cell) const
{
    return GetCell(GetRow(cell), GetColumn(cell));
}

CellIndex Map::GetCellIndex(const Position& position) const
{
    const Position pos = position - mRect.GetPosition();
    const float row = std::floor(static_cast<float>(pos.GetY()) / static_cast<float>(mCellSize));
    const float column = std::floor(static_cast<float>(pos.GetX()) / static_cast<float>(mCellSize));
    return CellIndex(static_cast<CellIndex::value_t>(row), static_cast<CellIndex::value_t>(column));
}

Position Map::GetCellCenterPos(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const
{
    return mRect.GetPosition() + (Position(columnIndex * mCellSize, rowIndex * mCellSize) + mCellSizeHalf);
}

Position Map::GetCellCenterPos(const CellIndex& index) const
{
    return GetCellCenterPos(GetRow(index), GetColumn(index));   
}

CellIndexArray Map::FindCells(const SpriteRegion& region) const
{
    static const size_t kMaxCellsCount = 6;
    CellIndexArray result;
    result.reserve(kMaxCellsCount);

    const auto addCell = [this, &result, &region](const CellIndex& cell)
    {
        const Position cellCenterPos = GetCellCenterPos(cell);
        if (region.HasPoint(cellCenterPos))
            result.push_back(cell);
    };

    // find cells that contain the region vertices
    PACMAN_CHECK_ERROR(mRect.IsIntersect(region), ErrorCode::InvalidState);
    SpriteRegion intersection = mRect.Intersection(region);
    const CellIndex leftTopPosCell = GetCellIndex(intersection.GetPosition());
    const CellIndex rightTopPosCell = GetCellIndex(intersection.GetRightTopPos());
    const CellIndex leftBottomPosCell = GetCellIndex(intersection.GetLeftBottomPos());
    const CellIndex rightBottomPosCell = GetCellIndex(intersection.GetRightBottomPos());

    // find cells

    // between leftTop and rightTop
    if (GetColumn(rightTopPosCell) - GetColumn(leftTopPosCell) > 1)
        addCell(GetNext(leftTopPosCell, MoveDirection::Right));

    // between leftBottom and rightBottom
    if (GetColumn(rightBottomPosCell) - GetColumn(leftBottomPosCell) > 1)
        addCell(GetNext(leftBottomPosCell, MoveDirection::Right));

    // between leftTop and leftBottom
    if (GetRow(leftBottomPosCell) - GetRow(leftTopPosCell) > 1)
        addCell(GetNext(leftTopPosCell, MoveDirection::Down));

    // between rightTop and rightBottom
    if (GetRow(rightBottomPosCell) - GetRow(rightTopPosCell) > 1)
        addCell(GetNext(rightTopPosCell, MoveDirection::Down));

    // between all (middle)
    if ((GetRow(rightBottomPosCell) - GetRow(leftTopPosCell) > 1) &&
        (GetColumn(rightBottomPosCell) - GetColumn(leftTopPosCell) > 1))
    {
        const CellIndex cellIndex(GetRow(leftTopPosCell) + 1, GetColumn(leftTopPosCell) + 1);
        addCell(cellIndex);
    }

    PACMAN_CHECK_ERROR((result.size() <= kMaxCellsCount) && result.size() > 0, ErrorCode::InvalidState);
    return result;
}

MapNeighborsInfo Map::GetDirectNeighbors(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const
{
    MapNeighborsInfo info;

    // left
    info.mLeft.mDirection = MoveDirection::Left;
    if (columnIndex == 0)
        info.mLeft.mCellType = MapCellType::Space;
    else
        info.mLeft.mCellType = GetCell(rowIndex, columnIndex - 1);

    // right
    info.mRight.mDirection = MoveDirection::Right;
    if (columnIndex >= (mColumnsCount - 1))
        info.mRight.mCellType = MapCellType::Space;
    else
        info.mRight.mCellType = GetCell(rowIndex, columnIndex + 1);

    // top
    info.mTop.mDirection = MoveDirection::Up;
    if (rowIndex == 0)
        info.mTop.mCellType = MapCellType::Space;
    else
        info.mTop.mCellType = GetCell(rowIndex - 1, columnIndex);

    // bottom
    info.mBottom.mDirection = MoveDirection::Down;
    if (rowIndex >= (mRowsCount - 1))
        info.mBottom.mCellType = MapCellType::Space;
    else
        info.mBottom.mCellType = GetCell(rowIndex + 1, columnIndex);

    return info;
}

MapNeighborsInfo Map::GetDirectNeighbors(const CellIndex& index) const
{
    return GetDirectNeighbors(GetRow(index), GetColumn(index));
}

FullMapNeighborsInfo Map::GetFullNeighbors(const CellIndex::value_t rowIndex, const CellIndex::value_t columnIndex) const
{
    FullMapNeighborsInfo info;
    info.mDirectInfo = GetDirectNeighbors(rowIndex, columnIndex);

    // left top
    if ((columnIndex == 0) || (rowIndex == 0))
        info.mLeftTop = MapCellType::Space;
    else
        info.mLeftTop = GetCell(rowIndex - 1, columnIndex - 1);

    // right top
    if ((columnIndex >= (mColumnsCount - 1)) || (rowIndex == 0))
        info.mRightTop = MapCellType::Space;
    else
        info.mRightTop = GetCell(rowIndex - 1, columnIndex + 1);

    // left bottom
    if ((columnIndex == 0) || (rowIndex >= (mRowsCount - 1)))
        info.mLeftBottom = MapCellType::Space;
    else
        info.mLeftBottom = GetCell(rowIndex + 1, columnIndex - 1);

    // right bottom
    if ((columnIndex >= (mColumnsCount - 1)) || (rowIndex >= (mRowsCount - 1)))
        info.mRightBottom = MapCellType::Space;
    else
        info.mRightBottom = GetCell(rowIndex + 1, columnIndex + 1);

    return info;
}

FullMapNeighborsInfo Map::GetFullNeighbors(const CellIndex& index) const
{
    return GetFullNeighbors(GetRow(index), GetColumn(index));
}

std::shared_ptr<Sprite> Map::GenerateSprite()
{
	// lets generate!
	TextureRegion textureRegion(Math::Vector2f::kZero, 0.0f, 0.0f);
	std::shared_ptr<Texture2D> texture = GenerateTexture(&textureRegion);

	AssetManager& assetManager = GetEngine().GetAssetManager();
	std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(AssetManager::kDefaultTextureVertexShader, AssetManager::kDefaultTextureFragmentShader);

	return std::make_shared<Sprite>(mRect, textureRegion, texture, shaderProgram, false);
}

std::shared_ptr<Texture2D> Map::GenerateTexture(TextureRegion* textureRegion)
{
    // expand to POT --> TODO: check that device supports this texture size  <--
    const Size textureWidth = NextPOT(mRect.GetWidth());
    const Size textureHeight = NextPOT(mRect.GetHeight());

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
				if ((neighbors.mLeft.mCellType == MapCellType::Empty) || (neighbors.mLeft.mCellType == MapCellType::Space))
					CutLeft(cellRegion, mCellSizeQuarter); // cut left side
				if ((neighbors.mRight.mCellType == MapCellType::Empty) || (neighbors.mRight.mCellType == MapCellType::Space))
					CutRight(cellRegion, mCellSizeQuarter); // cut right side
				if ((neighbors.mTop.mCellType == MapCellType::Empty) || (neighbors.mTop.mCellType == MapCellType::Space))
					CutTop(cellRegion, mCellSizeQuarter); // cut top side
				if ((neighbors.mBottom.mCellType == MapCellType::Empty) || (neighbors.mBottom.mCellType == MapCellType::Space))
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

			if (neighbors.mLeftTop == MapCellType::Empty &&
				neighbors.mDirectInfo.mLeft.mCellType == MapCellType::Wall &&
				neighbors.mDirectInfo.mTop.mCellType == MapCellType::Wall)
			{
				SpriteRegion artifactRegion(cellRegion.GetPosition(), mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.mLeftTop));
			}

			if (neighbors.mRightTop == MapCellType::Empty &&
				neighbors.mDirectInfo.mRight.mCellType == MapCellType::Wall &&
				neighbors.mDirectInfo.mTop.mCellType == MapCellType::Wall)
			{
				Position pos = cellRegion.GetRightTopPos();
				pos.SetX(pos.GetX() - mCellSizeQuarter);
				SpriteRegion artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.mRightTop));
			}

			if (neighbors.mLeftBottom == MapCellType::Empty &&
				neighbors.mDirectInfo.mLeft.mCellType == MapCellType::Wall &&
				neighbors.mDirectInfo.mBottom.mCellType == MapCellType::Wall)
			{
				Position pos = cellRegion.GetLeftBottomPos();
				pos.SetY(pos.GetY() - mCellSizeQuarter);
				SpriteRegion artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.mLeftBottom));
			}

			if (neighbors.mRightBottom == MapCellType::Empty &&
				neighbors.mDirectInfo.mRight.mCellType == MapCellType::Wall &&
				neighbors.mDirectInfo.mBottom.mCellType == MapCellType::Wall)
			{
				Position pos = cellRegion.GetRightBottomPos();
				pos.SetX(pos.GetX() - mCellSizeQuarter);
				pos.SetY(pos.GetY() - mCellSizeQuarter);
				SpriteRegion artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.mRightBottom));
			}
		}
	}
}

} // Pacman namespace
