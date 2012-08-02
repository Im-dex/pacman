#include "asset_manager.h"
#include "base.h"
#include "error.h"

#include <new>
#include <android/bitmap.h>

namespace Pacman {

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

std::shared_ptr<Texture2D> AssetManager::LoadTexture(JNIEnv* env, const char* name, const TextureFiltering filtering,
									 	 	 	     const TextureRepeat repeat)
{
	jobject bitmap = LoadTextureFromAssets(env, name);
	AndroidBitmapHolder bitmapHolder(env, bitmap);
	AndroidBitmapInfo info = bitmapHolder.GetInfo();
	PACMAN_CHECK_ERROR(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888, ErrorCode::BadPixelFormat);
	byte_t* pixels = bitmapHolder.LockPixels();
	return std::make_shared<Texture2D>(info.width, info.height, pixels, filtering, repeat);
}

jobject AssetManager::LoadTextureFromAssets(JNIEnv* env, const char* name)
{
	jclass cls = env->FindClass("com/imdex/pacman/NativeLib");
	PACMAN_CHECK_ERROR(cls != nullptr, ErrorCode::JClassNotFound);

	jmethodID mid = env->GetStaticMethodID(cls, "loadAssetBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
	PACMAN_CHECK_ERROR(mid != nullptr, ErrorCode::JFuncNotFound);

	jstring assetName = env->NewStringUTF(name);
	jobject bitmap = env->CallStaticObjectMethod(cls, mid, assetName);
	PACMAN_CHECK_ERROR(bitmap != nullptr, ErrorCode::InvalidResult);
	return bitmap;
}

} // Pacman namespace
