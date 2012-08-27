#include "spritesheet.h"
#include "error.h"
#include "texture.h"
#include "sprite.h"
#include "shader_program.h"
#include "json_helper.h"
#include "engine.h"
#include "asset_manager.h"

namespace Pacman {

SpriteSheet::SpriteSheet(const std::string& description)
{
	const Json::Value root = JsonHelper::ParseJson(description);
	PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

	const Json::Value image = root["image"];
	const Json::Value list = root["list"];
	PACMAN_CHECK_ERROR(image.isString() && list.isArray() &&
					   (list.size() > 0), ErrorCode::BadFormat);

	const std::string imageValue = image.asString();
	AssetManager& assetManager = GetEngine()->GetAssetManager();
	std::shared_ptr<Texture2D> texture = assetManager.LoadTexture(imageValue, TextureFiltering::None, TextureRepeat::None);

	for (size_t i = 0; i < list.size(); i++)
	{
		const Json::Value spriteObject = list[i];
		PACMAN_CHECK_ERROR(spriteObject.isObject(), ErrorCode::BadFormat);

		const Json::Value name = spriteObject["name"];
		const Json::Value vs = spriteObject["vs"];
		const Json::Value fs = spriteObject["fs"];
		const Json::Value alphaBlend = spriteObject["alpha_blend"];
		const Json::Value x = spriteObject["x"];
		const Json::Value y = spriteObject["y"];
		const Json::Value width = spriteObject["width"];
		const Json::Value height = spriteObject["height"];
		PACMAN_CHECK_ERROR(name.isString() && vs.isString() &&
						   fs.isString() && alphaBlend.isBool() && 
						   x.isNumeric() && y.isNumeric() &&
						   width.isNumeric() && height.isNumeric(), ErrorCode::BadFormat);

		const std::string nameValue = name.asString();
		const std::string vsValue = vs.asString();
		const std::string fsValue = fs.asString();
		const bool alphaBlendValue = alphaBlend.asBool();
		PACMAN_CHECK_ERROR((nameValue.size() > 0) && (vsValue.size() > 0) &&
						   (fsValue.size() > 0), ErrorCode::BadFormat);

		float xValue = x.asDouble();
		float yValue = y.asDouble();
		float widthValue = width.asDouble();
		float heightValue = height.asDouble();

		SpriteRegion spriteRegion(0, 0, texture->GetWidth() * widthValue, texture->GetHeight() * heightValue);
		TextureRegion textureRegion(xValue, yValue, widthValue, heightValue);
		std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(vsValue, fsValue);
		std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(spriteRegion, textureRegion, texture, shaderProgram, alphaBlendValue);
		mSprites.insert(std::make_pair(nameValue, sprite));
	}
}

std::shared_ptr<Sprite> SpriteSheet::GetSprite(const std::string& name)
{
	auto iter = mSprites.find(name);
	PACMAN_CHECK_ERROR(iter != mSprites.end(), ErrorCode::BadArgument);
	return iter->second;
}

} // Pacman namespace