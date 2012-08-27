#include "spritesheet.h"
#include "error.h"
#include "texture.h"
#include "shader_program.h"
#include "engine.h"
#include "asset_manager.h"

namespace Pacman {

static FORCEINLINE TextureFiltering ConvertFiltering(const std::string& filtering)
{
	if (filtering == "none")
		return TextureFiltering::None;
	if (filtering == "bilinear")
		return TextureFiltering::Bilinear;
	if (filtering == "trilinear")
		return TextureFiltering::Trilinear;

	PACMAN_CHECK_ERROR(false, ErrorCode::BadFormat);
}

SpriteSheet::SpriteSheet(const std::string& description)
{
	const Json::Value root = JsonHelper::ParseJson(description);
	PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

	const Json::Value image = root["image"];
	const Json::Value filtering = root["filtering"];
	const Json::Value list = root["list"];
	PACMAN_CHECK_ERROR(image.isString() && filtering.isString() && 
					   list.isArray() && (list.size() > 0), ErrorCode::BadFormat);

	const std::string imageValue = image.asString();
	const std::string filteringValue = filtering.asString();
	PACMAN_CHECK_ERROR((imageValue.size() > 0) && (filteringValue.size() > 0), ErrorCode::BadFormat);

	AssetManager& assetManager = GetEngine()->GetAssetManager();
	mTexture = assetManager.LoadTexture(imageValue, ConvertFiltering(filteringValue), TextureRepeat::None);

	for (size_t i = 0; i < list.size(); i++)
	{
		const Json::Value spriteObject = list[i];
		PACMAN_CHECK_ERROR(spriteObject.isObject(), ErrorCode::BadFormat);

		const Json::Value name = spriteObject["name"];
		PACMAN_CHECK_ERROR(name.isString(), ErrorCode::BadFormat);

		const std::string nameValue = name.asString();
		PACMAN_CHECK_ERROR(nameValue.size() > 0, ErrorCode::BadFormat);

		mSprites.insert(std::make_pair(nameValue, spriteObject));
	}
}

std::shared_ptr<Sprite> SpriteSheet::MakeSprite(const std::string& name, const SpriteRegion& region)
{
	auto iter = mSprites.find(name);
	PACMAN_CHECK_ERROR(iter != mSprites.end(), ErrorCode::BadArgument);
	const Json::Value spriteObject = iter->second;

	const Json::Value vs = spriteObject["vs"];
	const Json::Value fs = spriteObject["fs"];
	const Json::Value alphaBlend = spriteObject["alpha_blend"];
	const Json::Value x = spriteObject["x"];
	const Json::Value y = spriteObject["y"];
	const Json::Value width = spriteObject["width"];
	const Json::Value height = spriteObject["height"];
	PACMAN_CHECK_ERROR(vs.isString() && fs.isString() && alphaBlend.isBool() && 
					   x.isNumeric() && y.isNumeric() && width.isNumeric() &&
					   height.isNumeric(), ErrorCode::BadFormat);

	const std::string vsValue = vs.asString();
	const std::string fsValue = fs.asString();
	const bool alphaBlendValue = alphaBlend.asBool();
	PACMAN_CHECK_ERROR((vsValue.size() > 0) && (fsValue.size() > 0), ErrorCode::BadFormat);

	float xValue = x.asDouble();
	float yValue = y.asDouble();
	float widthValue = width.asDouble();
	float heightValue = height.asDouble();

	AssetManager& assetManager = GetEngine()->GetAssetManager();
	TextureRegion textureRegion(xValue, yValue, widthValue, heightValue);
	std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(vsValue, fsValue);
	return std::make_shared<Sprite>(region, textureRegion, mTexture, shaderProgram, alphaBlendValue);
}

} // Pacman namespace