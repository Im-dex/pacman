#include "asset_manager.h"

#include <new>
#include <algorithm>
#include <memory>
#include <android/bitmap.h>

#include "engine.h"
#include "error.h"
#include "color.h"
#include "texture.h"
#include "shader_program.h"
#include "spritesheet.h"
#include "jni_utility.h"
#include "json_helper.h"
#include "utils.h"

namespace Pacman {

const std::string AssetManager::kDefaultColorVertexShader         = "def_color_shader.vs";
const std::string AssetManager::kDefaultStaticColorVertexShader   = "def_static_color_shader.vs";
const std::string AssetManager::kDefaultColorFragmentShader       = "def_color_shader.fs";
const std::string AssetManager::kDefaultTextureVertexShader       = "def_texture_shader.vs";
const std::string AssetManager::kDefaultStaticTextureVertexShader = "def_static_texture_shader.vs";
const std::string AssetManager::kDefaultTextureFragmentShader     = "def_texture_shader.fs";

class AndroidBitmapHolder
{
public:

	AndroidBitmapHolder() = delete;
	AndroidBitmapHolder(JNIEnv* env, jobject bitmap)
		: mEnv(env), mBitmap(bitmap), mIsLocked(false)
	{
	}

	AndroidBitmapHolder(const AndroidBitmapHolder&) = delete;
    AndroidBitmapHolder(AndroidBitmapHolder&&) = delete;
	~AndroidBitmapHolder()
	{
		UnlockPixels();
	}

	AndroidBitmapHolder& operator= (const AndroidBitmapHolder&) = delete;
	void* operator new (size_t) throw (std::bad_alloc) = delete;
	void* operator new (size_t, const std::nothrow_t&) throw() = delete;
	void* operator new[] (size_t) throw (std::bad_alloc) = delete;
	void* operator new[] (size_t, const std::nothrow_t&) throw() = delete;

	AndroidBitmapInfo GetInfo() const
	{
		AndroidBitmapInfo info;
		int res = AndroidBitmap_getInfo(mEnv, mBitmap, &info);
		PACMAN_CHECK_ERROR(res == ANDROID_BITMAP_RESUT_SUCCESS);
		return info;
	}

	const byte_t* LockPixels()
	{
		void* pixels = nullptr;
		int res = AndroidBitmap_lockPixels(mEnv, mBitmap, &pixels);
		PACMAN_CHECK_ERROR(res == ANDROID_BITMAP_RESUT_SUCCESS);
		mIsLocked = true;
		return static_cast<const byte_t*>(pixels);
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
	const size_t dotPos = name.find_last_of('.');
	PACMAN_CHECK_ERROR((dotPos != std::string::npos) && (dotPos < name.size()));

    return MakeString(name.substr(0, dotPos),                      // add name
                      "@", multiplier,                             // add multiplier
                      "x.", name.substr(dotPos + 1, name.size())); // add extension
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

	const std::string finalName = (mMultiplier > 0) ? ApplyMultiplier(name, mMultiplier)
                                                    : name;

	jobject bitmap = LoadBitmapFromAssets(finalName); // try to load bitmap with multiplier
	if ((bitmap == nullptr) && (mMultiplier > 0))
		bitmap = LoadBitmapFromAssets(name); // after try base
	PACMAN_CHECK_ERROR(bitmap != nullptr);

	AndroidBitmapHolder bitmapHolder(env, bitmap);
	const AndroidBitmapInfo info = bitmapHolder.GetInfo();

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

	const byte_t* pixels = bitmapHolder.LockPixels();
	return std::make_shared<Texture2D>(info.width, info.height, pixels, filtering, repeat, pixelFormat);
}

std::shared_ptr<ShaderProgram> AssetManager::LoadShaderProgram(const std::string& vertexShaderName, const std::string& fragmentShaderName)
{
	const auto iter = mShaderPrograms.find(vertexShaderName + fragmentShaderName);
	if (iter != mShaderPrograms.end())
	{
		const std::shared_ptr<ShaderProgram> shader = iter->second.lock();
		if (shader != nullptr)
			return shader;
		else
			mShaderPrograms.erase(iter);
	}

	const std::string vertexShader = LoadTextFile(vertexShaderName);
	const std::string fragmentShader = LoadTextFile(fragmentShaderName);
	PACMAN_CHECK_ERROR((vertexShader.size() > 0) && (fragmentShader.size() > 0));

	const std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
	shader->Link();
	mShaderPrograms.insert(std::make_pair(vertexShaderName + fragmentShaderName, shader));
	return shader;
}

std::shared_ptr<SpriteSheet> AssetManager::LoadSpriteSheet(const std::string& name)
{
    const std::string jsonData = LoadTextFile(name);
    PACMAN_CHECK_ERROR(jsonData.size() > 0);

    const JsonHelper::Value root(jsonData);

    typedef EnumType<TextureFiltering>::value TextureFilteringValueT;
    const std::string image = root.GetValue<std::string>("image");
    const TextureFilteringValueT filtering = root.GetValue<TextureFilteringValueT>("filtering");
    const JsonHelper::Array list  = root.GetValue<JsonHelper::Array>("list");
    PACMAN_CHECK_ERROR((image.size() > 0) && (list.GetSize() > 0));

    std::shared_ptr<Texture2D> texture = LoadTexture(image, MakeEnum<TextureFiltering>(filtering), TextureRepeat::None);

    NamedSpriteInfoArray namedSpritesInfo;
    namedSpritesInfo.reserve(list.GetSize());

    for (const JsonHelper::Value& sprite : list)
    {
        const std::string name = sprite.GetValue<std::string>("name");
        const std::string vs   = sprite.GetValue<std::string>("vs");
        const std::string fs   = sprite.GetValue<std::string>("fs");
        const bool alphaBlend  = sprite.GetValue<bool>("alpha_blend");
        PACMAN_CHECK_ERROR((name.size() > 0) && (vs.size() > 0) && (fs.size() > 0));

        const float x = sprite.GetValue<float>("x");
        const float y = sprite.GetValue<float>("y");
        const float width = sprite.GetValue<float>("width");
        const float height = sprite.GetValue<float>("height");

        const SpriteInfo spriteInfo  
        {
            TextureRegion(x, y, width, height),
                          vs,
                          fs,
                          alphaBlend
        };

        namedSpritesInfo.push_back(std::make_pair(name, spriteInfo));
    }

    return std::make_shared<SpriteSheet>(texture, namedSpritesInfo);
}

std::string AssetManager::LoadTextFile(const std::string& name)
{
	JNIEnv* env = JNI::GetEnv();

	jobject byteArray = LoadFileFromAssets(name);
	PACMAN_CHECK_ERROR(byteArray != nullptr);

	const char* buf = static_cast<const char*>(env->GetDirectBufferAddress(byteArray));
	PACMAN_CHECK_ERROR(buf != nullptr);

	const jlong capacity = env->GetDirectBufferCapacity(byteArray);
	return std::string(buf, capacity);
}

} // Pacman namespace
