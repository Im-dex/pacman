#include "asset_manager.h"
#include "base.h"
#include "engine.h"
#include "error.h"
#include "color.h"
#include "jni_utility.h"

#include <new>
#include <tuple>
#include <algorithm>
#include <memory>
#include <sstream>
#include <android/bitmap.h>

namespace Pacman {

static const uint8_t kTileSizes[4][4] = 
{
//			   ldpi	   mdpi	   hdpi	  xhdpi
/* Small */		8,		0,		16,		0,
/* Normal */	8,		16,		16,		32,
/* Large */		16,		32,		0,		0,
/* XLarge */	16,		32,		64,		64
};

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

jobject LoadBitmapFromAssets(const std::string& name)
{
	jstring assetName = JNI::MakeUTF8String(name.c_str());
	jobject bitmap = JNI::CallStaticObjectMethod("com/imdex/pacman/NativeLib", "loadAssetBitmap",
												 "(Ljava/lang/String;)Landroid/graphics/Bitmap;", assetName);
	PACMAN_CHECK_ERROR(bitmap != nullptr, ErrorCode::BadArgument);
	return bitmap;
}

jobject LoadFileFromAssets(const char* name)
{
	jstring assetName = JNI::MakeUTF8String(name);
	jobject byteBuffer = JNI::CallStaticObjectMethod("com/imdex/pacman/NativeLib", "loadAssetFile",
													 "(Ljava/lang/String;)Ljava/nio/ByteBuffer;", assetName);
	PACMAN_CHECK_ERROR(byteBuffer != nullptr, ErrorCode::BadArgument);
	return byteBuffer;
}

//=================================================================================================================

AssetManager::AssetManager()
{
	ScreenSize screenSize = Engine::GetInstance()->GetScreenSize();
	ScreenDensity screenDensity = Engine::GetInstance()->GetScreenDensity();
	mTileSize = kTileSizes[static_cast<uint8_t>(screenSize)][static_cast<uint8_t>(screenDensity)];
	PACMAN_CHECK_ERROR(mTileSize != 0, ErrorCode::UnsupportedDevice);

	std::stringstream folderStream("");
	folderStream << (size_t)mTileSize << "/";
	mAssetFolder = folderStream.str();
	LOGI("TileSize: %u, assFol: %s", mTileSize, mAssetFolder.c_str());
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture(const char* name, const TextureFiltering filtering,
									 	 	 	     const TextureRepeat repeat)
{
	JNIEnv* env = JNI::GetEnv();

	jobject bitmap = LoadBitmapFromAssets(BuildAssetPath(name));
	PACMAN_CHECK_ERROR(bitmap != nullptr, ErrorCode::InvalidResult);
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

std::string AssetManager::LoadTextFile(const char* name)
{
	return LoadTextFileFromRoot(BuildAssetPath(name).c_str());
}

std::string AssetManager::LoadTextFileFromRoot(const char* name)
{
	JNIEnv* env = JNI::GetEnv();

	jobject byteArray = LoadFileFromAssets(name);
	PACMAN_CHECK_ERROR(byteArray != nullptr, ErrorCode::InvalidResult);
	char* buf = static_cast<char*>(env->GetDirectBufferAddress(byteArray));
	PACMAN_CHECK_ERROR(buf != nullptr, ErrorCode::InvalidResult);
	jlong capacity = env->GetDirectBufferCapacity(byteArray);

	return std::string(buf, capacity);
}

std::string AssetManager::BuildAssetPath(const char* name)
{
	return mAssetFolder + name;
}


} // Pacman namespace
