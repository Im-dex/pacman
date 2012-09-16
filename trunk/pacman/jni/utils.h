#pragma once

#include <sstream>

#include "base.h"

namespace Pacman {

// internal compiler error when instant! TODO: submit crash (NDK r8b)
/*template <typename T, typename U>
bool TestFlag(const T data, const U flag)
{
	return (data & static_cast<T>(flag)) == static_cast<T>(flag);
}*/

// round up the value to next Power Of Two value
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

//=============================================================================

static FORCEINLINE float RoundToNearHalf(const float value)
{
	return static_cast<float>((static_cast<int>(value * 2.0f)) * 0.5f);
}

//=============================================================================

static void WriteValue(const std::ostringstream& stream)
{}

template <typename First, typename... Args>
static void WriteValue(std::ostringstream& stream, const First first, const Args... args)
{
    stream << first;
    WriteValue(stream, args...);
}

template <typename T>
static void WriteValue(std::ostringstream& stream, const T var)
{
    stream << var;
}

template <typename... Args>
static std::string MakeString(Args... args)
{
    std::ostringstream stream;
    WriteValue(stream, args...);
    return stream.str();
}

} // Pacman namespace