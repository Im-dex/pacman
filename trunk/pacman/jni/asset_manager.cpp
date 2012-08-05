#include "asset_manager.h"
#include "base.h"
#include "error.h"
#include "color.h"

#include <new>
#include <algorithm>
#include <memory>
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

static const byte_t kEmpty = static_cast<byte_t>('0');
static const byte_t kWall = static_cast<byte_t>('1');
static const byte_t kDoor = static_cast<byte_t>('2');

std::shared_ptr<Texture2D> AssetManager::LoadMap(JNIEnv* env, const char* name, const TextureFiltering filtering,
											  	 const size_t screenWidth, const size_t screenHeigth)
{
	/*static const size_t kDefaultMapWidth = 224;
	static const size_t kDefaultMapHeigth = 248;
	static const size_t kDefaultMapCellsCountInRow = 28;
	static const size_t kDefaultMapCellsCountInColumn = 31;
	static const size_t kDefaultMapCellsWidth = 8;
	static const size_t kDefaultMapCellsHeigth = 8;
	static const size_t kBytesPerPixel = sizeof(byte_t) * 3; // RGB

	static const Color kWallColor = Color::kBlue;
	static const Color kEmptyColor = Color::kBlack;
	static const Color kDoorColor = Color::kGreen;

	const size_t widthAspectRatio = std::abs(screenWidth / kDefaultMapWidth);
	const size_t heigthAspectRatio = std::abs(screenHeigth / kDefaultMapHeigth);
	const size_t mapScale = std::min(widthAspectRatio, heigthAspectRatio);

	const size_t mapWidth = kDefaultMapWidth * mapScale;
	const size_t mapHeigth = kDefaultMapHeigth * mapScale;

	const size_t screenExtraWidth = screenWidth - mapWidth;
	const size_t screenExtraHeigth = screenHeigth - mapHeigth;

	const size_t paddingLeft = screenExtraWidth / 2;
	const size_t paddingRight = paddingLeft;

	const size_t paddingTop = screenExtraHeigth / 2;
	const size_t paddingBottom = paddingTop;

	const size_t mapCellsWidth = kDefaultMapCellsWidth * mapScale;
	const size_t mapCellsHeigth = kDefaultMapCellsHeigth * mapScale;

	const size_t mapPixelsWidth = kDefaultMapCellsCountInRow * mapCellsWidth;
	const size_t mapPixelsHeigth = kDefaultMapCellsCountInColumn * mapCellsHeigth;
	const size_t mapPixelsCount = mapPixelsWidth * mapPixelsHeigth;
	const size_t mapPixelsBytesTotal = mapPixelsCount * kBytesPerPixel;
*/
	//=======================================================================================================

/*	jobject byteArray = LoadFileFromAssets(env, name);
	byte_t* buf = static_cast<byte_t*>(env->GetDirectBufferAddress(byteArray));
	jlong capacity = env->GetDirectBufferCapacity(byteArray);

	std::unique_ptr<byte_t[]> mapPixelData(new byte_t[mapPixelsBytesTotal]);

	size_t cellsWrited = 0;
	for (size_t i = 0; i < kDefaultMapCellsCountInRow; i++)
	{
		for (size_t j = 0; j < kDefaultMapCellsCountInColumn; j++)
		{
			byte_t* writeBufPos = mapPixelData.get() + (cellsWrited * kBytesPerPixel);
			byte_t cell = buf[i * kDefaultMapCellsCountInRow + j];

			switch (cell)
			{
			case kEmpty:
				kEmptyColor.Fill(writeBufPos, 3);
				break;
			case kWall:
				kWallColor.Fill(writeBufPos, 3);
				break;
			case kDoor:
				kDoorColor.Fill(writeBufPos, 3);
				break;
			default:
				PACMAN_CHECK_ERROR(false, ErrorCode::BadFormat);
				break;
			}
		}
	}

	return std::make_shared<Texture2D>(screenWidth, screenHeigth, mapPixelData.get(), TextureFiltering::None, TextureRepeat::None, PixelFormat::RGB_565);*/
	return nullptr;
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

jobject AssetManager::LoadFileFromAssets(JNIEnv* env, const char* name)
{
	jclass cls = env->FindClass("com/imdex/pacman/NativeLib");
	PACMAN_CHECK_ERROR(cls != nullptr, ErrorCode::JClassNotFound);

	jmethodID mid = env->GetStaticMethodID(cls, "loadAssetFile", "(Ljava/lang/String;)Ljava/nio/ByteBuffer;");
	PACMAN_CHECK_ERROR(mid != nullptr, ErrorCode::JFuncNotFound);

	jstring assetName = env->NewStringUTF(name);
	jobject byteBuffer = env->CallStaticObjectMethod(cls, mid, assetName);
	PACMAN_CHECK_ERROR(byteBuffer != nullptr, ErrorCode::InvalidResult);
	return byteBuffer;
}

} // Pacman namespace
