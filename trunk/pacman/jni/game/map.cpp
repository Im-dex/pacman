#include "map.h"

#include <cassert>
#include <complex>
#include <algorithm>

#include "base.h"
#include "error.h"
#include "color.h"
#include "engine.h"
#include "scene_node.h"
#include "shader_program.h"
#include "asset_manager.h"
#include "renderer.h"
#include "texture.h"
#include "json_helper.h"
#include "scene_manager.h"
#include "rect.h"

namespace Pacman {

static const uint16_t kBaseCellSize = 8;

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

Map::Map(const std::string& textData, std::vector<DotType>& dotsInfo)
   : mRect(0, 0, 0, 0)
{
	ParseJsonData(textData, dotsInfo);

    // calc map parameters
    Engine* engine = GetEngine();
    Renderer& renderer = engine->GetRenderer();
    size_t multiplier = engine->GetAssetManager().GetMultiplier();
    
    mCellSize = kBaseCellSize * multiplier;
    assert(mCellSize / kBaseCellSize == multiplier);
    mCellSizeHalf = mCellSize / 2;
    mCellSizeQuarter = mCellSize / 4;
    
    const size_t viewportWidth = renderer.GetViewportWidth();
    const size_t viewportHeight = renderer.GetViewportHeight();

    const size_t mapWidth = mColumnsCount * mCellSize;
    const size_t mapHeight = mRowsCount * mCellSize;

    const size_t leftRightPadding = (viewportWidth - mapWidth) / 2;
    const size_t topBottomPadding = (viewportHeight - mapHeight) / 2;

    mRect = SpriteRegion(leftRightPadding, topBottomPadding, mapWidth, mapHeight);

    // make sprite
	std::shared_ptr<Sprite> sprite = GenerateSprite();
	mNode = std::make_shared<SceneNode>(sprite, SpritePosition::kZero);
}

void Map::AttachToScene(SceneManager& sceneManager)
{
	sceneManager.AttachNode(mNode);
}

MapCellType Map::GetCell(const uint16_t rowIndex, const uint16_t columnIndex) const
{
    PACMAN_CHECK_ERROR((rowIndex <= mRowsCount) && (columnIndex <= mColumnsCount), ErrorCode::BadArgument);
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
        info.left = MapCellType::Empty;
    else
        info.left = GetCell(rowIndex, columnIndex - 1);

    // right
    if (columnIndex >= (mColumnsCount - 1))
        info.right = MapCellType::Empty;
    else
        info.right = GetCell(rowIndex, columnIndex + 1);

    // top
    if (rowIndex == 0)
        info.top = MapCellType::Empty;
    else
        info.top = GetCell(rowIndex - 1, columnIndex);

    // bottom
    if (rowIndex >= (mRowsCount - 1))
        info.bottom = MapCellType::Empty;
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
        info.leftTop = MapCellType::Empty;
    else
        info.leftTop = GetCell(rowIndex - 1, columnIndex - 1);

    // right top
    if ((columnIndex >= (mColumnsCount - 1)) || (rowIndex == 0))
        info.rightTop = MapCellType::Empty;
    else
        info.rightTop = GetCell(rowIndex - 1, columnIndex + 1);

    // left bottom
    if ((columnIndex == 0) || (rowIndex >= (mRowsCount - 1)))
        info.leftBottom = MapCellType::Empty;
    else
        info.leftBottom = GetCell(rowIndex + 1, columnIndex - 1);

    // right bottom
    if ((columnIndex >= (mColumnsCount - 1)) || (rowIndex >= (mRowsCount - 1)))
        info.rightBottom = MapCellType::Empty;
    else
        info.rightBottom = GetCell(rowIndex + 1, columnIndex + 1);

    return info;
}

FullMapNeighborsInfo Map::GetFullNeighbors(const CellIndex& index) const
{
    return GetFullNeighbors(index.GetX(), index.GetY());
}

void Map::ParseJsonData(const std::string& data, std::vector<DotType>& dotsInfo)
{
    const Json::Value root = JsonHelper::ParseJson(data);
    PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

    const Json::Value rowsCount = root["rowsCount"];
    PACMAN_CHECK_ERROR(rowsCount.isNumeric(), ErrorCode::BadFormat);

    mRowsCount = static_cast<const uint16_t>(rowsCount.asUInt());

    const Json::Value cells = root["cells"];
    PACMAN_CHECK_ERROR(cells.isArray(), ErrorCode::BadFormat);
    PACMAN_CHECK_ERROR(cells.size() % mRowsCount == 0, ErrorCode::BadFormat);

    dotsInfo.reserve(cells.size());

    for (size_t i = 0; i < cells.size(); i++)
    {
        const Json::Value cell = cells[i];
        PACMAN_CHECK_ERROR(cell.isNumeric(), ErrorCode::BadFormat);

        uint8_t value = static_cast<uint8_t>(cell.asUInt());
        PACMAN_CHECK_ERROR(value < kCellTypesCount + kDotTypesCount, ErrorCode::BadFormat);

        DotType dot = DotType::None;
        if ((value == static_cast<uint8_t>(DotType::Small)) || (value == static_cast<uint8_t>(DotType::Big)))
        {
            dot = static_cast<DotType>(value);
            value = static_cast<uint8_t>(MapCellType::Empty);
        }

        mCells.push_back(static_cast<MapCellType>(value));
        dotsInfo.push_back(dot);
    }

    mColumnsCount = mCells.size() / mRowsCount;
}

std::shared_ptr<Sprite> Map::GenerateSprite()
{
	// lets generate!
	TextureRegion textureRegion(Math::Vector2f::kZero, 0.0f, 0.0f);
	std::shared_ptr<Texture2D> texture = GenerateTexture(&textureRegion);

	AssetManager& assetManager = GetEngine()->GetAssetManager();
	std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(AssetManager::kDefaultTextureVertexShader, AssetManager::kDefaultTextureFragmentShader);

	SpriteRegion region(mRect.GetPosition(), mRect.GetWidth(), mRect.GetHeight());
	return std::make_shared<Sprite>(region, textureRegion, texture, shaderProgram, false);
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
				if (neighbors.left == MapCellType::Empty) // cut left side
					CutLeft(cellRegion, mCellSizeQuarter);
				if (neighbors.right == MapCellType::Empty) // cut right side
					CutRight(cellRegion, mCellSizeQuarter);
				if (neighbors.top == MapCellType::Empty) // cut top side
					CutTop(cellRegion, mCellSizeQuarter);
				if (neighbors.bottom == MapCellType::Empty) // cut bottom side
					CutBottom(cellRegion, mCellSizeQuarter);
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
