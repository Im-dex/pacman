#include "asset_manager.h"
#include "base.h"
#include "engine.h"
#include "error.h"
#include "color.h"
#include "texture.h"
#include "shader_program.h"
#include "jni_utility.h"

#include <new>
#include <algorithm>
#include <memory>
#include <sstream>
#include <android/bitmap.h>

namespace Pacman {

const std::string AssetManager::kDefaultColorVertexShader = "def_color_shader.vs";
const std::string AssetManager::kDefaultStaticColorVertexShader = "def_static_color_shader.vs";
const std::string AssetManager::kDefaultColorFragmentShader = "def_color_shader.fs";
const std::string AssetManager::kDefaultTextureVertexShader = "def_texture_shader.vs";
const std::string AssetManager::kDefaultStaticTextureVertexShader = "def_static_texture_shader.vs";
const std::string AssetManager::kDefaultTextureFragmentShader = "def_texture_shader.fs";

class AndroidBitmapHolder
{
public:

	AndroidBitmapHolder() = delete;
	AndroidBitmapHolder(JNIEnv* env, jobject bitmap)
		: mEnv(env), mBitmap(bitmap), mIsLocked(false)
	{
	}

	AndroidBitmapHolder(const AndroidBitmapHolder&) = delete;
	~AndroidBitmapHolder()
	{
		UnlockPixels();
	}

	AndroidBitmapHolder& operator= (const AndroidBitmapHolder&) = delete;
	void* operator new (size_t) throw (std::bad_alloc) = delete;
	void* operator new (size_t, const std::nothrow_t&) throw() = delete;
	void* operator new[] (size_t) throw (std::bad_alloc) = delete;
	void* operator new[] (size_t, const std::nothrow_t&) throw() = delete;

	AndroidBitmapInfo GetInfo()
	{
		AndroidBitmapInfo info;
		int res = AndroidBitmap_getInfo(mEnv, mBitmap, &info);
		PACMAN_CHECK_ERROR(res == ANDROID_BITMAP_RESUT_SUCCESS, ErrorCode::AndroidAPICallFailed);
		return info;
	}

	byte_t* LockPixels()
	{
		void* pixels = nullptr;
		int res = AndroidBitmap_lockPixels(mEnv, mBitmap, &pixels);
		PACMAN_CHECK_ERROR(res == ANDROID_BITMAP_RESUT_SUCCESS, ErrorCode::AndroidAPICallFailed);
		mIsLocked = true;
		return static_cast<byte_t*>(pixels);
	}

	void UnlockPixels()
	{
		if (mIsLocked)
		{
			AndroidBitmap_unlockPixels(mEnv, mBitmap);
			mIsLocked = false;
		}
	}

private:

	JNIEnv* mEnv;
	jobject mBitmap;
	bool 	mIsLocked;
};

std::string ApplyMultiplier(const std::string& name, const size_t multiplier)
{
	std::ostringstream result;
	size_t dotPos = name.find_last_of('.');
	PACMAN_CHECK_ERROR((dotPos != std::string::npos) && (dotPos < name.size()), ErrorCode::BadArgument);

	result << name.substr(0, dotPos); // add name
	result << '@' << multiplier << "x."; // add multiplier
	result << name.substr(dotPos + 1, name.size()); // add extension

	return result.str();
}

jobject LoadBitmapFromAssets(const std::string& name)
{
	jstring assetName = JNI::MakeUTF8String(name.c_str());
	return JNI::CallStaticObjectMethod("com/imdex/pacman/NativeLib", "loadAssetBitmap",
									   "(Ljava/lang/String;)Landroid/graphics/Bitmap;", assetName);
}

jobject LoadFileFromAssets(const std::string& name)
{
	jstring assetName = JNI::MakeUTF8String(name.c_str());
	return JNI::CallStaticObjectMethod("com/imdex/pacman/NativeLib", "loadAssetFile",
									   "(Ljava/lang/String;)Ljava/nio/ByteBuffer;", assetName);
}

//=================================================================================================================

std::shared_ptr<Texture2D> AssetManager::LoadTexture(const std::string& name, const TextureFiltering filtering,
									 	 	 	     const TextureRepeat repeat)
{
	JNIEnv* env = JNI::GetEnv();

	std::string finalName = name;
	if (mMultiplier > 0)
		finalName = ApplyMultiplier(name, mMultiplier);

	jobject bitmap = LoadBitmapFromAssets(finalName); // try to load bitmap with multiplier
	if ((bitmap == nullptr) && (mMultiplier > 0))
		bitmap = LoadBitmapFromAssets(name); // after try base
	PACMAN_CHECK_ERROR(bitmap != nullptr, ErrorCode::BadArgument);

	AndroidBitmapHolder bitmapHolder(env, bitmap);
	AndroidBitmapInfo info = bitmapHolder.GetInfo();

	PixelFormat pixelFormat = PixelFormat::None;
	switch (info.format)
	{
	case ANDROID_BITMAP_FORMAT_A_8:
		pixelFormat = PixelFormat::A_8;
		break;
	case ANDROID_BITMAP_FORMAT_RGB_565:
		pixelFormat = PixelFormat::RGB_565;
		break;
	case ANDROID_BITMAP_FORMAT_RGBA_4444:
		pixelFormat = PixelFormat::RGBA_4444;
		break;
	case ANDROID_BITMAP_FORMAT_RGBA_8888:
		pixelFormat = PixelFormat::RGBA_8888;
		break;
	case ANDROID_BITMAP_FORMAT_NONE:
		pixelFormat = PixelFormat::None;
		break;
	}

	byte_t* pixels = bitmapHolder.LockPixels();
	return std::make_shared<Texture2D>(info.width, info.height, pixels, filtering, repeat, pixelFormat);
}

std::shared_ptr<ShaderProgram> AssetManager::LoadShaderProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
{
	auto iter = mShaderPrograms.find(vertexShaderName + fragmentShaderName);
	if (iter != mShaderPrograms.end())
	{
		std::shared_ptr<ShaderProgram> shader = iter->second.lock();
		if (shader != nullptr)
			return shader;
		else
			mShaderPrograms.erase(iter);
	}

	std::string vertexShader = LoadTextFile(vertexShaderName);
	std::string fragmentShader = LoadTextFile(fragmentShaderName);
	PACMAN_CHECK_ERROR((vertexShader.size() > 0) && (fragmentShader.size() > 0), ErrorCode::BadArgument);
	std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
	shader->Link();
	mShaderPrograms.insert(std::make_pair(vertexShaderName + fragmentShaderName, shader));
	return shader;
}

std::string AssetManager::LoadTextFile(const std::string& name)
{
	JNIEnv* env = JNI::GetEnv();

	jobject byteArray = LoadFileFromAssets(name);
	PACMAN_CHECK_ERROR(byteArray != nullptr, ErrorCode::BadArgument);
	char* buf = static_cast<char*>(env->GetDirectBufferAddress(byteArray));
	PACMAN_CHECK_ERROR(buf != nullptr, ErrorCode::InvalidResult);
	jlong capacity = env->GetDirectBufferCapacity(byteArray);

	return std::string(buf, capacity);
}

} // Pacman namespace
