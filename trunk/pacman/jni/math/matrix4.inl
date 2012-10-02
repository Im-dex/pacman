#include <complex>

namespace Pacman {
namespace Math {

template <typename T>
Matrix4<T> Matrix4<T>::Ortho(const T left, const T right, const T bottom, const T top, const T near, const T far)
{
	const T val1 = T(2)  / (right - left);
	const T val2 = T(2)  / (top - bottom);
	const T val3 = T(-2) / (far - near);

	const T tx = -(right + left) / (right - left);
	const T ty = -(top + bottom) / (top - bottom);
	const T tz = -(far + near) / (far - near);

	return Matrix4<T>(val1, T(0), T(0), tx,
					  T(0), val2, T(0), ty,
					  T(0), T(0), val3, tz,
					  T(0), T(0), T(0),	T(1));
}

template <typename T>
FORCEINLINE Matrix4<T>::Matrix4(const T m00, const T m01, const T m02, const T m03,
								const T m10, const T m11, const T m12, const T m13,
								const T m20, const T m21, const T m22, const T m23,
								const T m30, const T m31, const T m32, const T m33)
					  : mM00(m00), mM01(m01), mM02(m02), mM03(m03),
						mM10(m10), mM11(m11), mM12(m12), mM13(m13),
						mM20(m20), mM21(m21), mM22(m22), mM23(m23),
						mM30(m30), mM31(m31), mM32(m32), mM33(m33)
{
}



template <typename T>
FORCEINLINE Matrix4<T>::Matrix4(const T val)
					  : mM00(val), mM01(val), mM02(val), mM03(val),
						mM10(val), mM11(val), mM12(val), mM13(val),
						mM20(val), mM21(val), mM22(val), mM23(val),
						mM30(val), mM31(val), mM32(val), mM33(val)
{
}

template <typename T>
FORCEINLINE Matrix4<T>::Matrix4(const T* data)
					  : mM00(data[0]),  mM01(data[1]),  mM02(data[2]),  mM03(data[3]),
						mM10(data[4]),  mM11(data[5]),  mM12(data[6]),  mM13(data[7]),
						mM20(data[8]),  mM21(data[9]),  mM22(data[10]), mM23(data[11]),
						mM30(data[12]), mM31(data[13]), mM32(data[14]), mM33(data[15])
{
}

template <typename T>
FORCEINLINE Matrix4<T>::Matrix4(const Vector4<T>& vec1,  const Vector4<T>& vec2, const Vector4<T>& vec3, const Vector4<T>& vec4)
					  : mM00(vec1.GetX()), mM01(vec1.GetY()), mM02(vec1.GetZ()), mM03(vec1.GetW()),
						mM10(vec2.GetX()), mM11(vec2.GetY()), mM12(vec2.GetZ()), mM13(vec2.GetW()),
						mM20(vec3.GetX()), mM21(vec3.GetY()), mM22(vec3.GetZ()), mM23(vec3.GetW()),
						mM30(vec4.GetX()), mM31(vec4.GetY()), mM32(vec4.GetZ()), mM33(vec4.GetW())
{
}

template <typename T>
FORCEINLINE Matrix4<T>::Matrix4(const Matrix4<T>& other)
					  : mData(other.mData)
{
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator= (const Matrix4<T>& other) 
{
	if (this != &other)
	{
		mData = other.mData;
	}

	return *this;
}

template <typename T>
FORCEINLINE bool Matrix4<T>::operator== (const Matrix4<T>& other) const
{
	return (Comparator<T>::Equals(mM00, other.mM00) && Comparator<T>::Equals(mM01, other.mM01) &&
			Comparator<T>::Equals(mM02, other.mM02) && Comparator<T>::Equals(mM03, other.mM03) &&

			Comparator<T>::Equals(mM10, other.mM10) && Comparator<T>::Equals(mM11, other.mM11) &&
			Comparator<T>::Equals(mM12, other.mM12) && Comparator<T>::Equals(mM13, other.mM13) &&

			Comparator<T>::Equals(mM20, other.mM20) && Comparator<T>::Equals(mM21, other.mM21) &&
			Comparator<T>::Equals(mM22, other.mM22) && Comparator<T>::Equals(mM23, other.mM23) &&

			Comparator<T>::Equals(mM30, other.mM30) && Comparator<T>::Equals(mM31, other.mM31) &&
			Comparator<T>::Equals(mM32, other.mM32) && Comparator<T>::Equals(mM33, other.mM33));
}

template <typename T>
FORCEINLINE bool Matrix4<T>::operator!= (const Matrix4<T>& other) const
{
	return !(*this == other);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator+ (const Matrix4<T>& other) const
{
	return Matrix4<T>(mM00 + other.mM00, mM01 + other.mM01, mM02 + other.mM02, mM03 + other.mM03,
					  mM10 + other.mM10, mM11 + other.mM11, mM12 + other.mM12, mM13 + other.mM13,
					  mM20 + other.mM20, mM21 + other.mM21, mM22 + other.mM22, mM23 + other.mM23,
					  mM30 + other.mM30, mM31 + other.mM31, mM32 + other.mM32, mM33 + other.mM33);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator- () const
{
	return Matrix4<T>(-mM00, -mM01, -mM02, -mM03,
					  -mM10, -mM11, -mM12, -mM13,
					  -mM20, -mM21, -mM22, -mM23,
					  -mM30, -mM31, -mM32, -mM33);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator- (const Matrix4<T>& other) const
{
	return Matrix4<T>(mM00 - other.mM00, mM01 - other.mM01, mM02 - other.mM02, mM03 - other.mM03,
					  mM10 - other.mM10, mM11 - other.mM11, mM12 - other.mM12, mM13 - other.mM13,
					  mM20 - other.mM20, mM21 - other.mM21, mM22 - other.mM22, mM23 - other.mM23,
					  mM30 - other.mM30, mM31 - other.mM31, mM32 - other.mM32, mM33 - other.mM33);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator* (const Matrix4<T>& other) const
{
	return Matrix4<T>(mM00 * other.mM00 + mM01 * other.mM10 + mM02 * other.mM20 + mM03 * other.mM30,
					  mM00 * other.mM01 + mM01 * other.mM11 + mM02 * other.mM21 + mM03 * other.mM31,
					  mM00 * other.mM02 + mM01 * other.mM12 + mM02 * other.mM22 + mM03 * other.mM32,
					  mM00 * other.mM03 + mM01 * other.mM13 + mM02 * other.mM23 + mM03 * other.mM33,
					  
					  mM10 * other.mM00 + mM11 * other.mM10 + mM12 * other.mM20 + mM13 * other.mM30,
					  mM10 * other.mM01 + mM11 * other.mM11 + mM12 * other.mM21 + mM13 * other.mM31,
					  mM10 * other.mM02 + mM11 * other.mM12 + mM12 * other.mM22 + mM13 * other.mM32,
					  mM10 * other.mM03 + mM11 * other.mM13 + mM12 * other.mM23 + mM13 * other.mM33,
					  
					  mM20 * other.mM00 + mM21 * other.mM10 + mM22 * other.mM20 + mM23 * other.mM30,
					  mM20 * other.mM01 + mM21 * other.mM11 + mM22 * other.mM21 + mM23 * other.mM31,
					  mM20 * other.mM02 + mM21 * other.mM12 + mM22 * other.mM22 + mM23 * other.mM32,
					  mM20 * other.mM03 + mM21 * other.mM13 + mM22 * other.mM23 + mM23 * other.mM33,
					  
					  mM30 * other.mM00 + mM31 * other.mM10 + mM32 * other.mM20 + mM33 * other.mM30,
					  mM30 * other.mM01 + mM31 * other.mM11 + mM32 * other.mM21 + mM33 * other.mM31,
					  mM30 * other.mM02 + mM31 * other.mM12 + mM32 * other.mM22 + mM33 * other.mM32,
					  mM30 * other.mM03 + mM31 * other.mM13 + mM32 * other.mM23 + mM33 * other.mM33);
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator+= (const Matrix4<T>& other)
{
	return *this = (*this + other);
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator-= (const Matrix4<T>& other)
{
	return *this = (*this - other);
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator*= (const Matrix4<T>& other)
{
	return *this = (*this * other);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator+ (const T val) const
{
	return *this + Matrix4<T>(val);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator- (const T val) const
{
	return *this - Matrix4<T>(val);
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::operator* (const T val) const
{
	return *this * Matrix4<T>(val);
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator+= (const T val)
{
	return *this = (*this + Matrix4<T>(val));
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator-= (const T val)
{
	return *this = (*this - Matrix4<T>(val));
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::operator*= (const T val)
{
	return *this = (*this * Matrix4<T>(val));
}

template <typename T>
FORCEINLINE T* Matrix4<T>::operator[] (const size_t rowIndex)
{
	return mRowData[rowIndex].data();
}

template <typename T>
FORCEINLINE const T* Matrix4<T>::operator[] (const size_t rowIndex) const
{
	return mRowData[rowIndex].data();
}

template <typename T>
FORCEINLINE T* Matrix4<T>::GetRawData() 
{
	return mData.data();
}

template <typename T>
FORCEINLINE const T* Matrix4<T>::GetRawData() const 
{
	return mData.data();
}

template <typename T>
Matrix4<T> Matrix4<T>::Inverse() const
{
	const T a9 = mM20 * mM31 - mM21 * mM30;
	const T aa = mM20 * mM32 - mM22 * mM30;
	const T a8 = mM20 * mM33 - mM23 * mM30;

	const T a2 = mM21 * mM32 - mM22 * mM31;
	const T a5 = mM21 * mM33 - mM23 * mM31;
	const T a3 = mM22 * mM33 - mM23 * mM32;

	const T t1 = mM11 * a3 - mM12 * a5 + mM13 * a2;
	const T t2 = mM10 * a3 - mM12 * a8 + mM13 * aa;
	const T t3 = mM10 * a5 - mM11 * a8 + mM13 * a9;
	const T t4 = mM10 * a2 - mM11 * aa + mM12 * a9;
	
	T det = mM00 * t1 - mM01 * t2 + mM02 * t3 - mM03 * t4;

	if (std::abs(det) < (std::numeric_limits<T>::epsilon())) return kIdentity;
	det = T(1) / det;

	const T a0 = mM10 * mM21 - mM11 * mM20;
	const T af = mM10 * mM31 - mM11 * mM30;
	const T ac = mM11 * mM22 - mM12 * mM21;
	const T a6 = mM11 * mM32 - mM12 * mM31;
				  				  
	const T ab = mM12 * mM23 - mM13 * mM22;
	const T a7 = mM12 * mM33 - mM13 * mM32;
	const T ae = mM10 * mM23 - mM13 * mM20;
	const T ad = mM10 * mM33 - mM13 * mM30;
				  				  
	const T b0 = mM11 * mM23 - mM13 * mM21;
	const T b1 = mM10 * mM32 - mM12 * mM30;
	const T b2 = mM10 * mM22 - mM12 * mM20;
	const T b3 = mM11 * mM33 - mM13 * mM31;

	const T m00 = det * t1;
	const T m01 = -det * (mM01 * a3 - mM02 * a5 + mM03 * a2);
	const T m02 = det * (mM01 * a7 - mM02 * b3 + mM03 * a6);
	const T m03 = -det * (mM01 * ab - mM02 * b0 + mM03 * ac);

	const T m10 = -det * t2;
	const T m11 = det * (mM00 * a3 - mM02 * a8 + mM03 * aa);
	const T m12 = -det * (mM00 * a7 - mM02 * ad + mM03 * b1);
	const T m13 = det * (mM00 * ab - mM02 * ae + mM03 * b2);

	const T m20 = det * t3;
	const T m21 = -det * (mM00 * a5 - mM01 * a8 + mM03 * a9);
	const T m22 = det * (mM00 * b3 - mM01 * ad + mM03 * af);
	const T m23 = -det * (mM00 * b0 - mM01 * ae + mM03 * a0);

	const T m30 = -det * t4;
	const T m31 = det * (mM00 * a2 - mM01 * aa + mM02 * a9);
	const T m32 = -det * (mM00 * a6 - mM01 * b1 + mM02 * af);
	const T m33 = det * (mM00 * ac - mM01 * b2 + mM02 * a0);

	return Matrix4<T>(m00, m01, m02, m03,
					  m10, m11, m12, m13,
					  m20, m21, m22, m23,
					  m30, m31, m32, m33);
}

/*template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::TranslationInverse()
{
	return Matrix4<T>(T(1)	, T(0)	, T(0)	, T(0),
					  T(0)	, T(1)	, T(0)	, T(0),
					  T(0)	, T(0)	, T(1)	, T(0),
					  -m_M03, -m_M13, -m_M23, T(1));
}

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::ScaleInverse()
{
	return Matrix4<T>(T(1) / m_M00, T(0)		, T(0)		  , T(0),
					  T(0)		  , T(1) / m_M11, T(0)		  , T(0),
					  T(0)		  , T(0)		, T(1) / m_M22, T(0),
					  T(0)		  , T(0)		, T(0)		  , T(1));
}*/

template <typename T>
FORCEINLINE Matrix4<T> Matrix4<T>::Transpose() const
{
	return Matrix4<T>(mM00, mM10, mM20, mM30,
					  mM01, mM11, mM21, mM31,
					  mM02, mM12, mM22, mM32,
					  mM03, mM13, mM23, mM33);
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::TranslateX(const T x)
{
	mM03 += x;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::TranslateY(const T y)
{
	mM13 += y;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::TranslateZ(const T z)
{
	mM23 += z;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::Translate(const T x, const T y, const T z)
{
	mM03 += x;
	mM13 += y;
	mM23 += z;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::RotateX(const T x)
{
	T angleSin = std::sin(x);
	T angleCos = std::cos(x);

	mM11 = angleCos;  mM12 = angleSin;
	mM21 = -angleSin; mM22 = angleCos;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::RotateY(const T y)
{
	T angleSin = std::sin(y);
	T angleCos = std::cos(y);

	mM00 = angleCos;  mM02 = -angleSin;
	mM20 = -angleSin; mM22 = angleCos;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::RotateZ(const T z)
{
	T angleSin = std::sin(z);
	T angleCos = std::cos(z);

	mM00 = angleCos;  mM01 = angleSin;
	mM10 = -angleSin; mM11 = angleCos;
	return *this;
}

template <typename T>
Matrix4<T>& Matrix4<T>::Rotate(const T x, const T y, const T z)
{
	const T xCos = std::cos(x);
	const T xSin = std::sin(x);
	const T yCos = std::cos(y);
	const T ySin = std::sin(y);
	const T zCos = std::cos(z);
	const T zSin = std::sin(z);

	const T mul1 = xCos * ySin;
	const T mul2 = xSin * ySin;

	mM00 = yCos * zCos;           	   mM01 = -yCos * zSin;          	  mM02 = ySin;
	mM10 = mul2 * zCos + xCos * zSin;  mM11 = -mul2 * zSin + xCos * zCos; mM12 = -xSin * yCos;
	mM20 = -mul1 * zCos + xSin * zSin; mM21 = mul1 * zSin + xSin * zCos;  mM22 = xCos * yCos;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::ScaleX(const T x)
{
	mM00 *= x;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::ScaleY(const T y)
{
	mM11 *= y;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::ScaleZ(const T z)
{
	mM22 *= z;
	return *this;
}

template <typename T>
FORCEINLINE Matrix4<T>& Matrix4<T>::Scale(const T x, const T y, const T z)
{
	mM00 *= x;
	mM11 *= y;
	mM22 *= z;
	return *this;
}

/*template <typename T>
FORCEINLINE Vector4<T> Matrix4<T>::operator* (const Vector4<T>& vec) const
{
	return Vector4<T>(vec.m_X * m_M00 + vec.m_Y * m_M01 + vec.m_Z * m_M02 + vec.m_W * m_M03,
					  vec.m_X * m_M10 + vec.m_Y * m_M11 + vec.m_Z * m_M12 + vec.m_W * m_M13,
					  vec.m_X * m_M20 + vec.m_Y * m_M21 + vec.m_Z * m_M22 + vec.m_W * m_M23,
					  vec.m_X * m_M30 + vec.m_Y * m_M31 + vec.m_Z * m_M32 + vec.m_W * m_M33);
}

template <typename T>
FORCEINLINE Vector4<T> operator* (const Vector4<T>& vec, const Matrix4<T>& mat)
{
	return Vector4<T>(vec.m_X * m_M00 + vec.m_Y * m_M10 + vec.m_Z * m_M20 + vec.m_W * m_M30,
					  vec.m_X * m_M01 + vec.m_Y * m_M11 + vec.m_Z * m_M21 + vec.m_W * m_M31,
					  vec.m_X * m_M02 + vec.m_Y * m_M12 + vec.m_Z * m_M22 + vec.m_W * m_M32,
					  vec.m_X * m_M03 + vec.m_Y * m_M13 + vec.m_Z * m_M23 + vec.m_W * m_M33);
}*/

template <typename T>
Matrix4<T> Matrix4<T>::MakeTranslationX(const T x)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).TranslateX(x);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeTranslationY(const T y)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).TranslateY(y);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeTranslationZ(const T z)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).TranslateZ(z);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeTranslation(const T x, const T y, const T z)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).Translate(x, y, z);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationX(const T x)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).RotateX(x);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationY(const T y)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).RotateY(y);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationZ(const T z)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).RotateZ(z);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotation(const T x, const T y, const T z)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).Rotate(x, y, z);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeScaleX(const T x)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).ScaleX(x);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeScaleY(const T y)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).ScaleY(y);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeScaleZ(const T z)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).ScaleZ(z);
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeScale(const T x, const T y, const T z)
{
    return Matrix4<T>(Matrix4<T>::kIdentity).Scale(x, y, z);
}

} // Math namespace
} // Pacman namespace
