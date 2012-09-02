#pragma once

#include <cstdint>
#include <type_traits>
#include <android/log.h>

//#define ADRENO_PROFILER_COMPATIBILITY
#define PACMAN_DEBUG_MAP_TEXTURE

#ifdef __GNUC__
	#define FORCEINLINE __attribute__((always_inline))
	#define ALIGN(align) __attribute__((aligned((align))))
#else
	#error "Unknown compiler"
#endif

#define  LOG_TAG    "NativeLib"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace Pacman {

typedef uint8_t byte_t;

// internal compiler error when instant! TODO: submit crash (NDK r8b)
/*template <typename T, typename U>
bool TestFlag(const T data, const U flag)
{
	return (data & static_cast<T>(flag)) == static_cast<T>(flag);
}*/

// round up the value to next PowerOf Two value
// ATTENTION!! 32-bit only!
static FORCEINLINE size_t NextPOT(size_t value)
{
    --value;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return value + 1;
}

static FORCEINLINE float RoundToNearHalf(const float value)
{
	return static_cast<float>((static_cast<int>(value * 2.0f)) * 0.5f);
}

} // Pacman namespace
