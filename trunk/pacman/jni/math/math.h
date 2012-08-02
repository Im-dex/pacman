#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

#define PACMAN_CHECK_ARITHMETIC_TYPE\
		static_assert(std::is_arithmetic<T>::value, "Wrong type");

namespace Pacman {
namespace Math {

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
		return std::abs<T>(lhs - rhs) <= std::numeric_limits<T>::epsilon();
	}
};

} // Math namespace
} // Pacman namespace
