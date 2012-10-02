#pragma once

#include <cstdint>

#include "engine_config.h"

#ifdef __GNUC__
	#define FORCEINLINE __attribute__((always_inline))
#else
	#error "Unknown compiler"
#endif

#ifndef PACMAN_WORLD_SIZE
    #error "Invalid engine_config.h"
#endif

#ifdef NDEBUG
    #ifdef PACMAN_ADRENO_PROFILER_COMPATIBILITY
        #undef PACMAN_ADRENO_PROFILER_COMPATIBILITY
    #endif
    #ifdef PACMAN_DEBUG_MAP_TEXTURE
        #undef PACMAN_DEBUG_MAP_TEXTURE
    #endif
#endif

namespace Pacman {

#if PACMAN_WORLD_SIZE == PACMAN_WORLD_SMALL
    typedef uint16_t Size;
    typedef int32_t  SizeOffset;
#elif PACMAN_WORLD_SIZE == PACMAN_WORLD_LARGE
    typedef uint32_t Size;
    typedef int64_t  SizeOffset;
#else
    #error "Invalid engine_config.h"
#endif

typedef uint8_t byte_t;

} // Pacman namespace
