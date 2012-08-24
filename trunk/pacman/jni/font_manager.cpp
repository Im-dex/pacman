#include "font_manager.h"
#include "json_helper.h"
#include "engine.h"
#include "asset_manager.h"
#include "error.h"
#include "texture.h"

namespace Pacman {

static const char* kDefaultFomtConfigName = "font.json";

static const char kVertexShader2[] = "attribute vec4 vPosition;"
									 "attribute vec2 vTexCoords;\n"
									 "\n"
									 "uniform mat4 mModelProjectionMatrix;\n"
									 "uniform vec4 vTextColor;\n"
									 "\n"
									 "varying vec2 vVertTexCoords;\n"
									 "varying vec4 vVertTextColor;\n"
									 "void main() {\n"
									 "	gl_Position = mModelProjectionMatrix * vPosition;\n"
									 "	vVertTexCoords = vTexCoords;\n"
									 "}\n";

static const char kFragmentShader2[] = "precision mediump float;\n"
										"uniform sampler2D colorTexture;\n"
										"varying vec2 vVertTexCoords;\n"
										"varying vec4 vVertTextColor;\n"
										"void main() {\n"
										"	gl_FragColor = vVertTextColor;\n"
										"	gl_FragColor.w = texture2D(colorTexture, vVertTexCoords).w;\n"
										"}\n";

FontManager::FontManager()
{
	//AssetManager& assetManager = Engine::GetInstance()->GetAssetManager();
	//std::string configData = assetManager.LoadTextFileFromRoot(kDefaultFomtConfigName);

	//Json::Value root = JsonHelper::ParseJson(configData);
	//PACMAN_CHECK_ERROR(root != Json::Value::null, ErrorCode::BadFormat);

	//mASCIIstart = static_cast<size_t>(root["ASCIIstart"].asUInt());
	//mASCIIend = static_cast<size_t>(root["ASCIIend"].asUInt());
	//const std::string fontBitmap = root["font"].asString();

	//mTexture = assetManager.LoadTexture(fontBitmap.c_str(), TextureFiltering::None, TextureRepeat::None);

	// shader here

	//const uint8_t tileSize = assetManager.GetTileSize();
	//const size_t charsCount = mASCIIend - mASCIIstart;
	//const size_t charsInRow = mTexture->GetWidth() / tileSize;
	//const size_t charsInColumn = mTexture->GetHeight() / tileSize;
	//PACMAN_CHECK_ERROR(charsInRow * charsInColumn >= charsCount, ErrorCode::BadFormat);

	//for (size_t i = 0; i < charsCount; i++)
	//{
		//SpriteRegion region(SpriteRegion::Position(), tileSize, tileSize);
		//mSprites.push_back(std::make_shared<Sprite>());
	//}
}

std::shared_ptr<Sprite> FontManager::MakeString(const std::string& string, const Color textColor, const Math::Vector2f position) const
{
	//const size_t verticesCount = string.size() * k

	return nullptr;
}

} // Pacman namespace