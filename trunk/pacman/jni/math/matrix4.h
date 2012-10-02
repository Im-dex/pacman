#pragma once

#include <array>

#include "base.h"
#include "math.h"
#include "vector4.h"

namespace Pacman {
namespace Math {

template <typename T>
class Matrix4
{
public:

	PACMAN_CHECK_ARITHMETIC_TYPE;

	static const Matrix4<T> kZero;
	static const Matrix4<T> kIdentity;

    typedef T value_t;

	static Matrix4<T> Ortho(const T left, const T right, const T bottom, const T top, const T near, const T far);

    static Matrix4<T> MakeTranslationX(const T x);
    static Matrix4<T> MakeTranslationY(const T y);
    static Matrix4<T> MakeTranslationZ(const T z);
    static Matrix4<T> MakeTranslation(const T x, const T y, const T z);

    static Matrix4<T> MakeRotationX(const T x);
    static Matrix4<T> MakeRotationY(const T y);
    static Matrix4<T> MakeRotationZ(const T z);
    static Matrix4<T> MakeRotation(const T x, const T y, const T z);

    static Matrix4<T> MakeScaleX(const T x);
    static Matrix4<T> MakeScaleY(const T y);
    static Matrix4<T> MakeScaleZ(const T z);
    static Matrix4<T> MakeScale(const T x, const T y, const T z);

	explicit Matrix4() = default;
	explicit Matrix4(const T m00, const T m01, const T m02, const T m03,
					 const T m10, const T m11, const T m12, const T m13,
					 const T m20, const T m21, const T m22, const T m23,
					 const T m30, const T m31, const T m32, const T m33);

	explicit Matrix4(const T val);
	explicit Matrix4(const T* data);
	explicit Matrix4(const Vector4<T>& vec1,  const Vector4<T>& vec2, const Vector4<T>& vec3, const Vector4<T>& vec4);
	Matrix4(const Matrix4<T>& other);
	~Matrix4() = default;

	Matrix4<T>& operator= (const Matrix4<T>& other);

	bool operator== (const Matrix4<T>& other) const;
	bool operator!= (const Matrix4<T>& other) const;

	Matrix4<T> operator+ (const Matrix4<T>& other) const;
	Matrix4<T> operator- () const;
	Matrix4<T> operator- (const Matrix4<T>& other) const;
	Matrix4<T> operator* (const Matrix4<T>& other) const;

	Matrix4<T>& operator+= (const Matrix4<T>& other);
	Matrix4<T>& operator-= (const Matrix4<T>& other);
	Matrix4<T>& operator*= (const Matrix4<T>& other);

	Matrix4<T> operator+ (const T val) const;
	Matrix4<T> operator- (const T val) const;
	Matrix4<T> operator* (const T val) const;
									 
	Matrix4<T>& operator+= (const T val);
	Matrix4<T>& operator-= (const T val);
	Matrix4<T>& operator*= (const T val);

	T* operator[] (const size_t rowIndex);
	const T* operator[] (const size_t rowIndex) const;

	T* GetRawData();
	const T* GetRawData() const;

	Matrix4<T> Inverse() const;
/*	Matrix4<T>& TranslationInverse();
	Matrix4<T>& ScaleInverse();*/
	Matrix4<T> Transpose() const;

	Matrix4<T>& TranslateX(const T x);
	Matrix4<T>& TranslateY(const T y);
	Matrix4<T>& TranslateZ(const T z);
	Matrix4<T>& Translate(const T x, const T y, const T z);

	// angle must will be in the radians
	Matrix4<T>& RotateX(const T x);
	Matrix4<T>& RotateY(const T y);
	Matrix4<T>& RotateZ(const T z);
	Matrix4<T>& Rotate(const T x, const T y, const T z);

	Matrix4<T>& ScaleX(const T x);
	Matrix4<T>& ScaleY(const T y);
	Matrix4<T>& ScaleZ(const T z);
	Matrix4<T>& Scale(const T x, const T y, const T z);

private:

	union
	{
		struct  
		{
			T mM00, mM01, mM02, mM03,
			  mM10, mM11, mM12, mM13,
			  mM20, mM21, mM22, mM23,
			  mM30, mM31, mM32, mM33;
		};
		std::array<T, 16> mData;
		std::array<std::array<T, 4>, 4> mRowData;
	};
};

template <typename T>
const Matrix4<T> Matrix4<T>::kZero = Matrix4<T>(T(0), T(0), T(0), T(0),
                                                T(0), T(0), T(0), T(0),
                                                T(0), T(0), T(0), T(0),
                                                T(0), T(0), T(0), T(0));

template <typename T>
const Matrix4<T> Matrix4<T>::kIdentity = Matrix4<T>(T(1), T(0), T(0), T(0),
                                                    T(0), T(1), T(0), T(0),
                                                    T(0), T(0), T(1), T(0),
                                                    T(0), T(0), T(0), T(1));

typedef Matrix4<float>  Matrix4f;
typedef Matrix4<double> Matrix4d;
typedef Matrix4<size_t> Matrix4s;

} // Math namespace
} // Pacman namespace

#include "matrix4.inl"
