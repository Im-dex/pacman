#pragma once

#include <cstdint>

//#define ADRENO_PROFILER_COMPATIBILITY
#define PACMAN_DEBUG_MAP_TEXTURE

#ifdef __GNUC__
	#define FORCEINLINE __attribute__((always_inline))
	#define ALIGN(align) __attribute__((aligned((align))))
#else
	#error "Unknown compiler"
#endif

namespace Pacman {

typedef uint8_t byte_t;

} // Pacman namespace
