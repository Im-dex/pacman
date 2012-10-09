#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

#include "base.h"

#define PACMAN_CHECK_ARITHMETIC_TYPE\
    static_assert(std::is_arithmetic<T>::value, "Wrong type");

namespace Pacman {
namespace Math {

template <typename T>
T FORCEINLINE DegreesToRadians(const T degrees)
{
	PACMAN_CHECK_ARITHMETIC_TYPE;
	return degrees * (T(M_PI) / T(180));
}

template <typename T>
T FORCEINLINE RadiansToDegrees(const T radians)
{
	PACMAN_CHECK_ARITHMETIC_TYPE;
	return radians * (T(180) / T(M_PI));
}

template <typename T>
struct Comparator
{
	PACMAN_CHECK_ARITHMETIC_TYPE;

	static bool Greater(const T lhs, const T rhs)
	{
		return Equals(lhs, rhs) ? false : (lhs > rhs);
	}

	static bool GreaterOrEquals(const T lhs, const T rhs)
	{
		return Equals(lhs, rhs) ? true : (lhs > rhs);
	}

	static bool Less(const T lhs, const T rhs)
	{
		return Equals(lhs, rhs) ? false : (lhs < rhs);
	}

	static bool LessOrEquals(const T lhs, const T rhs)
	{
		return Equals(lhs, rhs) ? true : (lhs < rhs);
	}

	static bool Equals(const T lhs, const T rhs)
	{
		return std::abs(lhs - rhs) <= std::numeric_limits<T>::epsilon();
	}
};

} // Math namespace
} // Pacman namespace
