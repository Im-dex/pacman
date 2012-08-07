#pragma once

#include "base.h"
#include "texture.h"

#include <jni.h>
#include <memory>
#include <string>

namespace Pacman {

class AssetManager
{
public:

	AssetManager() = delete;
	AssetManager(const AssetManager&) = delete;
	~AssetManager() = delete;

	AssetManager& operator= (const AssetManager&) = delete;

	static std::shared_ptr<Texture2D> LoadTexture(JNIEnv* env, const char* name, const TextureFiltering filtering,
												  const TextureRepeat repeat);

	static std::string LoadTextFile(JNIEnv* env, const char* name);

private:

	static jobject LoadTextureFromAssets(JNIEnv* env, const char* name);
	static jobject LoadFileFromAssets(JNIEnv* env, const char* name);
};

} // Pacman namespace
