#pragma once

#include <cstdint>
#include <type_traits>
#include <android/log.h>

//#define ADRENO_PROFILER_COMPATIBILITY

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

// internal compiler error on instantiation! TODO: submit crash (NDK r8b)
/*template <typename T, typename U>
bool TestFlag(const T data, const U flag)
{
	return (data & static_cast<T>(flag)) == static_cast<T>(flag);
}*/

} // Pacman namespace
