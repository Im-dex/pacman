#pragma once

#include <array>

#include "base.h"
#include "math.h"

namespace Pacman {
namespace Math {

template <typename T>
class Vector4
{
public:

	PACMAN_CHECK_ARITHMETIC_TYPE;

	static const Vector4<T> kZero; 
	static const Vector4<T> kXAxis;
	static const Vector4<T> kYAxis;
	static const Vector4<T> kZAxis;
	static const Vector4<T> kWAxis;

    typedef T value_t;

	explicit Vector4() = default;
	explicit Vector4(const T x, const T y, const T z, const T w);
	explicit Vector4(const T xyzw);
	explicit Vector4(const T* data);
	Vector4(const Vector4<T>& other);
	~Vector4() = default;

	Vector4<T>& operator= (const Vector4<T>& other);

	bool operator== (const Vector4<T>& other) const;
	bool operator!= (const Vector4<T>& other) const;

	Vector4<T> operator+ (const Vector4<T>& other) const;
	Vector4<T> operator- () const;
	Vector4<T> operator- (const Vector4<T>& other) const;
	Vector4<T> operator* (const Vector4<T>& other) const;
	Vector4<T> operator/ (const Vector4<T>& other) const;

	Vector4<T>& operator+= (const Vector4<T>& other);
	Vector4<T>& operator-= (const Vector4<T>& other);
	Vector4<T>& operator*= (const Vector4<T>& other);
	Vector4<T>& operator/= (const Vector4<T>& other);

	Vector4<T> operator+ (const T xyzw) const;
	Vector4<T> operator- (const T xyzw) const;
	Vector4<T> operator* (const T xyzw) const;
	Vector4<T> operator/ (const T xyzw) const;
									   	
	Vector4<T>& operator+= (const T xyzw);
	Vector4<T>& operator-= (const T xyzw);
	Vector4<T>& operator*= (const T xyzw);
	Vector4<T>& operator/= (const T xyzw);

	T DotProduct(const Vector4<T>& other) const;
	//Vector4<T> CrossProduct(const Vector4<T>& other) const;

	// dot product
	T operator& (const Vector4<T>& other) const;
	// cross product
	//Vector4<T> operator^ (const Vector4<T>& other) const;

	T GetX() const;
	T GetY() const;
	T GetZ() const;
	T GetW() const;

	void SetX(const T x);
	void SetY(const T y);
	void SetZ(const T z);
	void SetW(const T w);
	void Set(const T x, const T y, const T z, const T w);
	void Set(const T xyzw);

	T Length() const;
	Vector4<T> Normalize() const;
	Vector4<T> Reverse() const;

private:

	union
	{
		struct
		{
			T mX;
			T mY;
			T mZ;
			T mW;
		};
		std::array<T, 4> mData;
	};
};

template <typename T>
const Vector4<T> Vector4<T>::kZero  = Vector4<T>(T(0), T(0), T(0), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kXAxis = Vector4<T>(T(1), T(0), T(0), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kYAxis = Vector4<T>(T(0), T(1), T(0), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kZAxis = Vector4<T>(T(0), T(0), T(1), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kWAxis = Vector4<T>(T(0), T(0), T(0), T(1));

template <typename T>
FORCEINLINE Vector4<T> operator* (const T val, const Vector4<T>& vec4)
{
	return vec4 * val;
}

typedef Vector4<float>  Vector4f;
typedef Vector4<double> Vector4d;
typedef Vector4<size_t> Vector4s;

} // Math namespace
} // Pacman namespace

#include "vector4.inl"
