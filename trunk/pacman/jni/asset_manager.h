#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "base.h"
#include "engine_forwdecl.h"

namespace Pacman {

// TODO: add context lost support
class AssetManager
{
public:

	// attrs: position, color; uniforms: modelView
	static const std::string kDefaultColorVertexShader;
	// attrs: position, color; uniforms: none
	static const std::string kDefaultStaticColorVertexShader;
	// varying: color
	static const std::string kDefaultColorFragmentShader;
	// attrs: position, texcoords; uniforms: modelView
	static const std::string kDefaultTextureVertexShader;
	// attrs: position, texcoords; uniforms: none
	static const std::string kDefaultStaticTextureVertexShader;
	// varying: texcoords
	static const std::string kDefaultTextureFragmentShader;

	AssetManager() = default;
	AssetManager(const AssetManager&) = delete;
	~AssetManager() = default;

	AssetManager& operator= (const AssetManager&) = delete;

	std::shared_ptr<Texture2D> LoadTexture(const std::string& name, const TextureFiltering filtering,
										   const TextureRepeat repeat);

	std::shared_ptr<ShaderProgram> LoadShaderProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName);

    std::unique_ptr<SpriteSheet> LoadSpriteSheet(const std::string& name);

	std::string LoadTextFile(const std::string& name);

	void SetMultiplier(const size_t multiplier)
	{
		mMultiplier = multiplier;
	}

	size_t GetMultiplier() const
	{
		return mMultiplier;
	}

private:
	
	size_t mMultiplier;
	std::unordered_map<std::string, std::weak_ptr<ShaderProgram>> mShaderPrograms;
};

} // Pacman namespace
