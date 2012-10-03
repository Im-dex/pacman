#pragma once

#include <type_traits>
#include <sstream>

#include "base.h"

namespace Pacman {

// internal compiler error on instantiation! TODO: submit crash (NDK r8b)
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
static void WriteValue(std::ostringstream& stream, const First& first, const Args... args)
{
    stream << first;
    WriteValue(stream, args...);
}

template <typename First, typename... Args>
static void WriteValue(std::ostringstream& stream, const First&& first, const Args... args)
{
    stream << std::forward<const First>(first);
    WriteValue(stream, args...);
}

template <typename T>
static void WriteValue(std::ostringstream& stream, const T& var)
{
    stream << var;
}

template <typename T>
static void WriteValue(std::ostringstream& stream, const T&& var)
{
    stream << std::forward<const T>(var);
}

template <typename... Args>
static std::string MakeString(Args... args)
{
    std::ostringstream stream("");
    WriteValue(stream, args...);
    return stream.str();
}

//===============================================================================

// temporary solution (reason: gcc 4.6.0 doesn't support std::underlying_type)
template<typename T>
struct UnderlyingType
{
    typedef typename std::conditional<T(-1) < T(0),
                                      typename std::make_signed<T>::type,
                                      typename std::make_unsigned<T>::type
                                     >::type value;
};

// get enum underlying type
template <typename T>
struct EnumType
{
    static_assert(std::is_enum<T>::value, "Only enum types is accepted");
    typedef typename UnderlyingType<T>::value value;
};

// cast enum to his underlying type (actual for strongly typed enums)
template <typename T>
FORCEINLINE typename EnumType<T>::value EnumCast(const T value)
{
    return static_cast<typename EnumType<T>::value>(value);
}

// make enum from this enum underlying type values
template <typename EnumT, typename T>
EnumT MakeEnum(const T value)
{
    static_assert(std::is_enum<EnumT>::value, "Only enum types is accepted");
    static_assert(std::is_same<T, typename EnumType<EnumT>::value>::value, "Enum underlying type is different with passed");
    return static_cast<EnumT>(value);
}

} // Pacman namespace