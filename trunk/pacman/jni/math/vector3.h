#pragma once

#include "base.h"
#include "math.h"

#include <array>

namespace Pacman {
namespace Math {

template <typename T>
class Vector3
{
public:

	PACMAN_CHECK_ARITHMETIC_TYPE;

	static const Vector3<T> kZero;
	static const Vector3<T> kXAxis;
	static const Vector3<T> kYAxis;
	static const Vector3<T> kZAxis;

	explicit Vector3() = default;
	explicit Vector3(const T x, const T y, const T z);
	explicit Vector3(const T xyz);
	explicit Vector3(const T* data);
	Vector3(const Vector3<T>& other);
	Vector3(Vector3<T>&& other);
	~Vector3() = default;

	Vector3<T>& operator= (const Vector3<T>& other);
	Vector3<T>& operator= (Vector3<T>&& other);

	bool operator== (const Vector3<T>& other) const;
	bool operator!= (const Vector3<T>& other) const;

	Vector3<T> operator+ (const Vector3<T>& other) const;
	Vector3<T> operator- () const;
	Vector3<T> operator- (const Vector3<T>& other) const;
	Vector3<T> operator* (const Vector3<T>& other) const;
	Vector3<T> operator/ (const Vector3<T>& other) const;

	Vector3<T>& operator+= (const Vector3<T>& other);
	Vector3<T>& operator-= (const Vector3<T>& other);
	Vector3<T>& operator*= (const Vector3<T>& other);
	Vector3<T>& operator/= (const Vector3<T>& other);

	Vector3<T> operator+ (const T xyz) const;
	Vector3<T> operator- (const T xyz) const;
	Vector3<T> operator* (const T xyz) const;
	Vector3<T> operator/ (const T xyz) const;
									   
	Vector3<T>& operator+= (const T xyz);
	Vector3<T>& operator-= (const T xyz);
	Vector3<T>& operator*= (const T xyz);
	Vector3<T>& operator/= (const T xyz);

	T DotProduct(const Vector3<T>& other) const;
	Vector3<T> CrossProduct(const Vector3<T>& other) const;

	// dot product
	T operator& (const Vector3<T>& other) const;
	// cross product
	Vector3<T> operator^ (const Vector3<T>& other) const;

	T GetX() const;
	T GetY() const;
	T GetZ() const;

	void SetX(const T x);
	void SetY(const T y);
	void SetZ(const T z);
	void Set(const T x, const T y, const T z);
	void Set(const T xyz);

	T Length() const;

	Vector3<T> Normalize() const;
	Vector3<T> Reverse() const;

private:

	union
	{
		struct
		{
			T mX;
			T mY;
			T mZ;
		};
		std::array<T, 3> mData;
	};
};

template <typename T>
FORCEINLINE Vector3<T> operator* (const T val, const Vector3<T>& vec3)
{
	return vec3 * val;
}

typedef Vector3<float>  Vector3f;
typedef Vector3<double> Vector3d;

} // Math namespace
} // Pacman namespace

#include "vector3.inl"
