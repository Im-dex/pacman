#include "map.h"
#include "base.h"
#include "error.h"
#include "color.h"
#include "scene_node.h"
#include "shader_program.h"
#include "texture.h"
#include "json/json.h"
#include "scene_manager.h"
#include "rect.h"

#include <complex>
#include <algorithm>

namespace Pacman {

typedef Rect<size_t> Region;

static const char kMapVertexShader[] = "attribute vec4 vPosition;"
									   "attribute vec2 vTexCoords;\n"
									   "uniform mat4 mModelProjectionMatrix;\n"
									   "varying vec2 vVertTexCoords;\n"
									   "void main() {\n"
									   "	gl_Position = mModelProjectionMatrix * vPosition;\n"
									   "	vVertTexCoords = vTexCoords;\n"
									   "}\n";

static const char kMapFragmentShader[] = "precision mediump float;\n"
									     "uniform sampler2D colorTexture;\n"
									     "varying vec2 vVertTexCoords;\n"
							  	  	     "void main() {\n"
							  	  	     "	gl_FragColor = texture2D(colorTexture, vVertTexCoords);\n"
							  	  	     "}\n";

static const Color kEmptyColor = Color::kBlack;
static const Color kWallColor = Color::kBlue;
static const Color kDoorColor = Color::kWhite;
static const Color kSpaceColor = Color::kBlack;
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
	case MapCellType::Space:
		return kSpaceColor;
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
		const size_t bufferOffset = ((i + region.GetY()) * bytesInRow) + (region.GetX() * kColorComponentsCount);
		memcpy(colorBuffer + bufferOffset, row.get(), bytesInRegionRow);
	}
}

// cut cutSize pixels from left
static FORCEINLINE void CutLeft(Region* region, const size_t cutSize)
{
	region->SetX(region->GetX() + cutSize);
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
	region->SetY(region->GetY() + cutSize);
	region->SetHeight(region->GetHeight() - cutSize);
}

// cut cutSize pixels from bottom
static FORCEINLINE void CutBottom(Region* region, const size_t cutSize)
{
	region->SetHeight(region->GetHeight() - cutSize);
}

//============================================================================================================================================

Map::Map()
   : mCells(),
     mRowsCount(0),
     mColumnsCount(0),
     mCellSize(0),
     mNode()
{
}

void Map::Load(const std::string& textData, const size_t screenWidth, const size_t screenHeight)
{
	Json::Value root;
	Json::Reader reader;
	bool result = reader.parse(textData, root, false);
	PACMAN_CHECK_ERROR(result, ErrorCode::BadFormat);

	mCellSize = static_cast<const uint8_t>(root["cellSize"].asUInt());
	mCellHalf = mCellSize / 2;
	mRowsCount = static_cast<const uint8_t>(root["rowsCount"].asUInt());

	const Json::Value cells = root["cells"];
	for (size_t i = 0; i < cells.size(); i++)
	{
		uint8_t value = static_cast<uint8_t>(cells[i].asUInt());
		PACMAN_CHECK_ERROR(value < kCellTypesCount, ErrorCode::BadFormat);
		mCells.push_back(static_cast<MapCellType>(value));
	}

	PACMAN_CHECK_ERROR(mCells.size() % mRowsCount == 0, ErrorCode::BadFormat);
	mColumnsCount = mCells.size() / mRowsCount;

	Math::Vector2f position = Math::Vector2f::kZero;
	Sprite sprite = GenerateSprite(screenWidth, screenHeight, &position);

	mNode = std::make_shared<SceneNode>(sprite, position);
}

void Map::AttachToScene(std::shared_ptr<SceneManager> sceneManager)
{
	sceneManager->AttachNode(mNode);
}

Sprite Map::GenerateSprite(const size_t screenWidth, const size_t screenHeight, Math::Vector2f* position)
{
	size_t mapWidth = mColumnsCount * mCellSize;
	size_t mapHeight = mRowsCount * mCellSize;

	const size_t widthRatio = screenWidth / mapWidth;
	const size_t heightRatio = screenHeight / mapHeight;
	const size_t ratio = std::min(widthRatio, heightRatio);

	// expand to maximum possible size for the current screen
	mapWidth  *= ratio;
	mapHeight *= ratio;
	mCellSize *= ratio;

	// expand to POT --> TODO: check that device supports this texture size  <--
	const size_t textureWidth = NextPOT(mapWidth);
	const size_t textureHeight = NextPOT(mapHeight);

	// calc padding and size
	// horizontal padding - always center
	// vertical padding always on top
	const size_t leftRightPadding = (screenWidth - mapWidth) / 2;
	*position = Math::Vector2f(leftRightPadding, 0.0f);

	// lets generate!
	Math::Vector2f leftTopTexCoord = Math::Vector2f::kZero;
	Math::Vector2f rightTopTexCoord = Math::Vector2f::kZero;
	Math::Vector2f leftBottomTexCoord = Math::Vector2f::kZero;
	Math::Vector2f rightBottomTexCoord = Math::Vector2f::kZero;

	auto texture = GenerateTexture(textureWidth, textureHeight, mapWidth, mapHeight, &leftTopTexCoord,
								   &rightTopTexCoord, &leftBottomTexCoord, &rightBottomTexCoord);

	auto shaderProgram = std::make_shared<ShaderProgram>(kMapVertexShader, kMapFragmentShader);
	shaderProgram->Link();

	return Sprite(mapWidth, mapHeight, leftTopTexCoord, rightTopTexCoord, leftBottomTexCoord,
				  rightBottomTexCoord, texture, shaderProgram);
}

std::shared_ptr<Texture2D> Map::GenerateTexture(const size_t textureWidth, const size_t textureHeight,
					 	  	  	  	  	  	    const size_t mapWidth, const size_t mapHeight,
					 	  	  	  	  	  	    Math::Vector2f* leftTopTexCoord, Math::Vector2f* rightTopTexCoord,
					 	  	  	  	  	  	    Math::Vector2f* leftBottomTexCoord, Math::Vector2f* rightBottomTexCoord)
{
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
			Region cellRegion(j * mCellSize, i * mCellSize, mCellSize, mCellSize);

			if (cell == MapCellType::Door)
			{
				// cut the ghost house door height
				CutTop(&cellRegion, mCellHalf);
				CutBottom(&cellRegion, mCellHalf);
			} 
			else if (cell == MapCellType::Wall)
			{
				NeighborsInfo neighbors = GetDirectNeighbors(i, j);
				if (neighbors.left == MapCellType::Empty) // cut left side
					CutLeft(&cellRegion, mCellHalf);
				if (neighbors.right == MapCellType::Empty) // cut right side
					CutRight(&cellRegion, mCellHalf);
				if (neighbors.top == MapCellType::Empty) // cut top side
					CutTop(&cellRegion, mCellHalf);
				if (neighbors.bottom == MapCellType::Empty) // cut bottom side
					CutBottom(&cellRegion, mCellHalf);
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
	Region rightRectangle(mapWidth, 0, textureWidth - mapWidth, mapHeight);
	FillRegion(buffer.get(), textureWidth, rightRectangle, kAlignColor);

	// fill bottom align rectangle
	Region bottomRectangle(0, mapHeight, textureWidth, textureHeight - mapHeight);
	FillRegion(buffer.get(), textureWidth, bottomRectangle, kAlignColor);

	//
	// calculate texcoords
	//
	const float u = static_cast<float>(mapWidth) / static_cast<float>(textureWidth);
	const float v = static_cast<float>(mapHeight) / static_cast<float>(textureHeight);

	*leftTopTexCoord = Math::Vector2f::kZero;
	*rightTopTexCoord = Math::Vector2f(u, 0.0f);
	*leftBottomTexCoord = Math::Vector2f(0.0f, v);
	*rightBottomTexCoord = Math::Vector2f(u, v);
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

			Region cellRegion(j * mCellSize, i * mCellSize, mCellSize, mCellSize);
			FullNeighborsInfo neighbors = GetFullNeighbors(i, j);

			if (neighbors.leftTop == MapCellType::Empty &&
				neighbors.directInfo.left == MapCellType::Wall &&
				neighbors.directInfo.top == MapCellType::Wall)
			{
				Region artifactRegion(cellRegion.GetX(), cellRegion.GetY(), mCellHalf, mCellHalf);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.leftTop));
			}

			if (neighbors.rightTop == MapCellType::Empty &&
				neighbors.directInfo.right == MapCellType::Wall &&
				neighbors.directInfo.top == MapCellType::Wall)
			{
				Region artifactRegion(cellRegion.GetX() + cellRegion.GetWidth() - mCellHalf, cellRegion.GetY(),
									  mCellHalf, mCellHalf);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.rightTop));
			}

			if (neighbors.leftBottom == MapCellType::Empty &&
				neighbors.directInfo.left == MapCellType::Wall &&
				neighbors.directInfo.bottom == MapCellType::Wall)
			{
				Region artifactRegion(cellRegion.GetX(), cellRegion.GetY() + cellRegion.GetHeight() - mCellHalf,
									  mCellHalf, mCellHalf);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.leftBottom));
			}

			if (neighbors.rightBottom == MapCellType::Empty &&
				neighbors.directInfo.right == MapCellType::Wall &&
				neighbors.directInfo.bottom == MapCellType::Wall)
			{
				Region artifactRegion(cellRegion.GetX() + cellRegion.GetWidth() - mCellHalf,
									  cellRegion.GetY() + cellRegion.GetHeight() - mCellHalf, mCellHalf, mCellHalf);
				FillRegion(buffer, textureWidth, artifactRegion, GetColor(neighbors.rightBottom));
			}
		}
	}
}

MapCellType Map::GetCell(const uint8_t rowIndex, const uint8_t columnIndex) const
{
	PACMAN_CHECK_ERROR((rowIndex <= mRowsCount) && (columnIndex <= mColumnsCount), ErrorCode::BadArgument);
	return mCells[(rowIndex * mColumnsCount) + columnIndex];
}

typename Map::NeighborsInfo Map::GetDirectNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const
{
	NeighborsInfo info;

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

typename Map::FullNeighborsInfo Map::GetFullNeighbors(const uint8_t rowIndex, const uint8_t columnIndex) const
{
	FullNeighborsInfo info;
	info.directInfo = GetDirectNeighbors(rowIndex, columnIndex);

	// left top
	if (columnIndex == 0 || rowIndex == 0)
		info.leftTop = MapCellType::Space;
	else
		info.leftTop = GetCell(rowIndex - 1, columnIndex - 1);

	// right top
	if (columnIndex >= (mColumnsCount - 1) || rowIndex == 0)
		info.rightTop = MapCellType::Space;
	else
		info.rightTop = GetCell(rowIndex - 1, columnIndex + 1);

	// left bottom
	if (columnIndex == 0 || rowIndex >= (mRowsCount - 1))
		info.leftBottom = MapCellType::Space;
	else
		info.leftBottom = GetCell(rowIndex + 1, columnIndex - 1);

	// right bottom
	if (columnIndex >= (mColumnsCount - 1) || rowIndex >= (mRowsCount - 1))
		info.rightBottom = MapCellType::Space;
	else
		info.rightBottom = GetCell(rowIndex + 1, columnIndex + 1);

	return info;
}

} // Pacman namespace
