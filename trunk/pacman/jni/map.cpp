#include "map.h"
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

#include <complex>
#include <algorithm>

namespace Pacman {

static const size_t kBaseCellSize = 8;

typedef Rect<size_t> Region;

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
static void FillRegion(byte_t* colorBuffer, const size_t rowWidth, const Region region, const Color color)
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
static FORCEINLINE void CutLeft(Region* region, const size_t cutSize)
{
	region->SetPosX(region->GetPosX() + cutSize);
	region->SetWidth(region->GetWidth() - cutSize);	
}

// cut cutSize pixels from right
static FORCEINLINE void CutRight(Region* region, const size_t cutSize)
{
	region->SetWidth(region->GetWidth() - cutSize);
}

// cut cutSize pixels from top
static FORCEINLINE void CutTop(Region* region, const size_t cutSize)
{
	region->SetPosY(region->GetPosY() + cutSize);
	region->SetHeight(region->GetHeight() - cutSize);
}

// cut cutSize pixels from bottom
static FORCEINLINE void CutBottom(Region* region, const size_t cutSize)
{
	region->SetHeight(region->GetHeight() - cutSize);
}

//============================================================================================================================================

Map::Map(const std::string& textData)
   : mRect(0, 0, 0, 0)
{
	ParseJsonData(textData);

    // calc map parameters
    Engine* engine = GetEngine();
    Renderer& renderer = engine->GetRenderer();
    size_t multiplier = engine->GetAssetManager().GetMultiplier();

    mCellSize = kBaseCellSize * multiplier;
    mCellSizeHalf = mCellSize / 2;
    mCellSizeQuarter = mCellSize / 4;
    
    const size_t viewportWidth = renderer.GetViewportWidth();
    const size_t viewportHeight = renderer.GetViewportHeight();

    const size_t mapWidth = mColumnsCount * mCellSize;
    const size_t mapHeight = mRowsCount * mCellSize;

    const size_t leftRightPadding = (viewportWidth - mapWidth) / 2;
    const size_t topBottomPadding = (viewportHeight - mapHeight) / 2;

    mRect = Rect<size_t>(leftRightPadding, topBottomPadding, mapWidth, mapHeight);

    // make sprite
	std::shared_ptr<Sprite> sprite = GenerateSprite();
	mNode = std::make_shared<SceneNode>(sprite, Math::Vector2f::kZero);
}

void Map::AttachToScene(SceneManager& sceneManager)
{
	sceneManager.AttachNode(mNode);
}

MapCellType Map::GetCell(const size_t rowIndex, const size_t columnIndex) const
{
    return GetCell(Math::Vector2s(rowIndex, columnIndex));
}

MapCellType Map::GetCell(const Math::Vector2s& cellIndices) const
{
    PACMAN_CHECK_ERROR((cellIndices.GetX() <= mRowsCount) && (cellIndices.GetY() <= mColumnsCount), ErrorCode::BadArgument);
    return mCells[(cellIndices.GetX() * mColumnsCount) + cellIndices.GetY()];
}

Math::Vector2s Map::GetCellPosition(const size_t rowIndex, const size_t columnIndex) const
{
    return GetCellPosition(Math::Vector2s(rowIndex, columnIndex));
}

Math::Vector2s Map::GetCellPosition(const Math::Vector2s& cell) const
{
    return mRect.GetPosition() + (cell * mCellSize) + mCellSizeHalf;
}

MapNeighborsInfo Map::GetDirectNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const
{
    return GetDirectNeighbors(Math::Vector2s(rowIndex, columnIndex));
}

MapNeighborsInfo Map::GetDirectNeighbors(const Math::Vector2s& cellIndices) const
{
    MapNeighborsInfo info;

    // left
    if (cellIndices.GetY() == 0)
        info.left = MapCellType::Empty;
    else
        info.left = GetCell(cellIndices.GetX(), cellIndices.GetY() - 1);

    // right
    if (cellIndices.GetY() >= (mColumnsCount - 1))
        info.right = MapCellType::Empty;
    else
        info.right = GetCell(cellIndices.GetX(), cellIndices.GetY() + 1);

    // top
    if (cellIndices.GetX() == 0)
        info.top = MapCellType::Empty;
    else
        info.top = GetCell(cellIndices.GetX() - 1, cellIndices.GetY());

    // bottom
    if (cellIndices.GetX() >= (mRowsCount - 1))
        info.bottom = MapCellType::Empty;
    else
        info.bottom = GetCell(cellIndices.GetX() + 1, cellIndices.GetY());

    return info;
}

FullMapNeighborsInfo Map::GetFullNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const
{
    return GetFullNeighbors(Math::Vector2s(rowIndex, columnIndex));
}

FullMapNeighborsInfo Map::GetFullNeighbors(const Math::Vector2s& cellIndices) const
{
    FullMapNeighborsInfo info;
    info.directInfo = GetDirectNeighbors(cellIndices.GetX(), cellIndices.GetY());

    // left top
    if ((cellIndices.GetY() == 0) || (cellIndices.GetX() == 0))
        info.leftTop = MapCellType::Empty;
    else
        info.leftTop = GetCell(cellIndices.GetX() - 1, cellIndices.GetY() - 1);

    // right top
    if ((cellIndices.GetY() >= (mColumnsCount - 1)) || (cellIndices.GetX() == 0))
        info.rightTop = MapCellType::Empty;
    else
        info.rightTop = GetCell(cellIndices.GetX() - 1, cellIndices.GetY() + 1);

    // left bottom
    if ((cellIndices.GetY() == 0) || (cellIndices.GetX() >= (mRowsCount - 1)))
        info.leftBottom = MapCellType::Empty;
    else
        info.leftBottom = GetCell(cellIndices.GetX() + 1, cellIndices.GetY() - 1);

    // right bottom
    if ((cellIndices.GetY() >= (mColumnsCount - 1)) || (cellIndices.GetX() >= (mRowsCount - 1)))
        info.rightBottom = MapCellType::Empty;
    else
        info.rightBottom = GetCell(cellIndices.GetX() + 1, cellIndices.GetY() + 1);

    return info;
}

void Map::ParseJsonData(const std::string& data)
{
    const Json::Value root = JsonHelper::ParseJson(data);
    PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

    const Json::Value rowsCount = root["rowsCount"];
    PACMAN_CHECK_ERROR(rowsCount.isNumeric(), ErrorCode::BadFormat);

    mRowsCount = static_cast<const uint8_t>(rowsCount.asUInt());

    const Json::Value cells = root["cells"];
    PACMAN_CHECK_ERROR(cells.isArray(), ErrorCode::BadFormat);
    PACMAN_CHECK_ERROR(cells.size() % mRowsCount == 0, ErrorCode::BadFormat);

    for (size_t i = 0; i < cells.size(); i++)
    {
        const Json::Value cell = cells[i];
        PACMAN_CHECK_ERROR(cell.isNumeric(), ErrorCode::BadFormat);

        uint8_t value = static_cast<uint8_t>(cell.asUInt());
        PACMAN_CHECK_ERROR(value < kCellTypesCount, ErrorCode::BadFormat);

        mCells.push_back(static_cast<MapCellType>(value));
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
			Region cellRegion(Region::Position(j * mCellSize, i * mCellSize), mCellSize, mCellSize);

			if (cell == MapCellType::Door)
			{
				// cut the ghost house door height
				CutTop(&cellRegion, mCellSizeQuarter);
				CutBottom(&cellRegion, mCellSizeQuarter);
			} 
			else if (cell == MapCellType::Wall)
			{
				MapNeighborsInfo neighbors = GetDirectNeighbors(i, j);
				if (neighbors.left == MapCellType::Empty) // cut left side
					CutLeft(&cellRegion, mCellSizeQuarter);
				if (neighbors.right == MapCellType::Empty) // cut right side
					CutRight(&cellRegion, mCellSizeQuarter);
				if (neighbors.top == MapCellType::Empty) // cut top side
					CutTop(&cellRegion, mCellSizeQuarter);
				if (neighbors.bottom == MapCellType::Empty) // cut bottom side
					CutBottom(&cellRegion, mCellSizeQuarter);
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
	Region rightRectangle(mRect.GetWidth(), 0, textureWidth - mRect.GetWidth(), mRect.GetHeight());
	FillRegion(buffer.get(), textureWidth, rightRectangle, kAlignColor);

	// fill bottom align rectangle
	Region bottomRectangle(0, mRect.GetHeight(), textureWidth, textureHeight - mRect.GetHeight());
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
	// draw small rectangle in the corner for to hide an artifact
	for (size_t i = 0; i < mRowsCount; i++)
	{
		for (size_t j = 0; j < mColumnsCount; j++)
		{
			MapCellType cell = GetCell(i, j);
			if (cell != MapCellType::Wall)
				continue;

			Region cellRegion(Region::Position(j * mCellSize, i * mCellSize), mCellSize, mCellSize);
			FullMapNeighborsInfo neighbors = GetFullNeighbors(i, j);

			if (neighbors.leftTop == MapCellType::Empty &&
				neighbors.directInfo.left == MapCellType::Wall &&
				neighbors.directInfo.top == MapCellType::Wall)
			{
				Region artifactRegion(cellRegion.GetPosition(), mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.leftTop));
			}

			if (neighbors.rightTop == MapCellType::Empty &&
				neighbors.directInfo.right == MapCellType::Wall &&
				neighbors.directInfo.top == MapCellType::Wall)
			{
				Region::Position pos = cellRegion.GetRightTopPos();
				pos.SetX(pos.GetX() - mCellSizeQuarter);
				Region artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.rightTop));
			}

			if (neighbors.leftBottom == MapCellType::Empty &&
				neighbors.directInfo.left == MapCellType::Wall &&
				neighbors.directInfo.bottom == MapCellType::Wall)
			{
				Region::Position pos = cellRegion.GetLeftBottomPos();
				pos.SetY(pos.GetY() - mCellSizeQuarter);
				Region artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.leftBottom));
			}

			if (neighbors.rightBottom == MapCellType::Empty &&
				neighbors.directInfo.right == MapCellType::Wall &&
				neighbors.directInfo.bottom == MapCellType::Wall)
			{
				Region::Position pos = cellRegion.GetRightBottomPos();
				pos.SetX(pos.GetX() - mCellSizeQuarter);
				pos.SetY(pos.GetY() - mCellSizeQuarter);
				Region artifactRegion(pos, mCellSizeQuarter, mCellSizeQuarter);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.rightBottom));
			}
		}
	}
}

} // Pacman namespace
