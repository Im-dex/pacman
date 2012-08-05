#include "map.h"
#include "base.h"
#include "error.h"
#include "color.h"
#include "scene_node.h"
#include "shader_program.h"
#include "texture.h"
#include "json/json.h"

#include <complex>
#include <algorithm>

namespace Pacman {

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
static const Color kAlignColor = Color::kRed;

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

Sprite Map::GenerateSprite(const size_t screenWidth, const size_t screenHeight, Math::Vector2f* position)
{
	size_t mapWidth = mRowsCount * mCellSize;
	size_t mapHeight = mColumnsCount * mCellSize;

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

	return Sprite(mapWidth, mapHeight, leftTopTexCoord, rightTopTexCoord, leftBottomTexCoord,
				  rightBottomTexCoord, texture, shaderProgram);
}

std::shared_ptr<Texture2D> Map::GenerateTexture(const size_t textureWidth, const size_t textureHeight,
					 	  	  	  	  	  	    const size_t mapWidth, const size_t mapHeight,
					 	  	  	  	  	  	    Math::Vector2f* leftTopTexCoord, Math::Vector2f* rightTopTexCoord,
					 	  	  	  	  	  	    Math::Vector2f* leftBottomTexCoord, Math::Vector2f* rightBottomTexCoord)
{
	static const size_t kColorComponentsCount = 3; // RGB

	const size_t bufferSize = textureWidth * textureHeight * kColorComponentsCount;
	const size_t rowBytesSize = textureWidth * kColorComponentsCount;
	std::unique_ptr<byte_t[]> buffer(new byte_t[bufferSize]);

	//
	// fill map
	//
	size_t rowsFilled = 0;
	size_t columnsFilled = 0;
	for (size_t i = 0; i < mCells.size(); i++)
	{
		Color color = GetColor(mCells[i]);
		const size_t dataOffset = (rowsFilled * rowBytesSize) + (columnsFilled * kColorComponentsCount);
		color.Fill(buffer.get() + dataOffset, kColorComponentsCount);

		if (++columnsFilled == mColumnsCount) // next row
		{
			++rowsFilled;
			columnsFilled = 0;
		}
	}

	//
	// fill align range
	//

	// fill right align rectangle
	for (size_t i = 0; i < mapHeight; i++)
	{
		for (size_t j = mapWidth; j < textureWidth; j += kColorComponentsCount)
		{
			const size_t dataOffset = (i * (textureWidth - mapWidth) * kColorComponentsCount) + j;
			kAlignColor.Fill(buffer.get() + dataOffset, kColorComponentsCount);
		}
	}

	// fill bottom align reactangle
	for (size_t i = mapHeight; i < textureHeight; i++)
	{
		for (size_t j = 0; j < textureWidth; j += kColorComponentsCount)
		{
			const size_t dataOffset = (i * (textureWidth - mapWidth) * kColorComponentsCount) + j;
			kAlignColor.Fill(buffer.get() + dataOffset, kColorComponentsCount);
		}
	}

	//
	// calculate texcoords
	//
	const float u = mapWidth / textureWidth;
	const float v = mapHeight / textureHeight;

	*leftTopTexCoord = Math::Vector2f::kZero;
	*rightTopTexCoord = Math::Vector2f(u, 0.0f);
	*leftBottomTexCoord = Math::Vector2f(0.0f, v);
	*rightBottomTexCoord = Math::Vector2f(u, v);

	//
	// make texture
	// //Texture2D(w, h, data, filt, repe, pixfmt);
	return std::make_shared<Texture2D>(textureWidth, textureHeight, buffer.get(), TextureFiltering::None,
									   TextureRepeat::None, PixelFormat::RGB_565);
}

} // Pacman namespace
