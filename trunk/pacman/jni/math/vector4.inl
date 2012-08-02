#include <cmath>
#include <algorithm>
#include <utility>

namespace Pacman {
namespace Math {

template <typename T>
FORCEINLINE Vector4<T>::Vector4(const T x, const T y, const T z, const T w)
					  : mX(x),
					    mY(y),
						mZ(z),
						mW(w)
{
}

template <typename T>
FORCEINLINE Vector4<T>::Vector4(const T xyzw)
					  : mX(xyzw),
					    mY(xyzw),
						mZ(xyzw),
						mW(xyzw)
{
}

template <typename T>
FORCEINLINE Vector4<T>::Vector4(const T* data)
					  : mX(data[0]),
					    mY(data[1]),
						mZ(data[2]),
						mW(data[3])
{
}

template <typename T>
FORCEINLINE Vector4<T>::Vector4(const Vector4<T>& other) 
					  : mData(other.mData)
{
}

template <typename T>
FORCEINLINE Vector4<T>::Vector4(Vector4<T>&& other)
{ 
	*this = std::move(other);
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator= (const Vector4<T>& other) 
{
	if (this != &other)
	{
		mData = other.mData;
	}

	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator= (Vector4<T>&& other)
{
	if (this != &other)
	{
		std::swap(mData, other.mData);
	}

	return *this;
}

template <typename T>
FORCEINLINE bool Vector4<T>::operator== (const Vector4<T>& other) const
{
	return (Comparator<T>::Equals(mX, other.mX) && Comparator<T>::Equals(mY, other.mY) &&
			Comparator<T>::Equals(mZ, other.mZ) && Comparator<T>::Equals(mW, other.mW));
}

template <typename T>
FORCEINLINE bool Vector4<T>::operator!= (const Vector4<T>& other) const
{
	return !(*this == other);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator+ (const Vector4<T>& other) const
{
	return Vector4<T>(mX + other.mX, mY + other.mY, mZ + other.mZ, mW + other.mW);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator- () const
{
	return Vector4<T>(-mX, -mY, -mZ, -mW);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator- (const Vector4<T>& other) const
{
	return Vector4<T>(mX - other.mX, mY - other.mY, mZ - other.mZ, mW - other.mW);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator* (const Vector4<T>& other) const
{
	return Vector4<T>(mX * other.mX, mY * other.mY, mZ * other.mZ, mW * other.mW);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator/ (const Vector4<T>& other) const
{
	return Vector4<T>(mX / other.mX, mY / other.mY, mZ / other.mZ, mW / other.mW);
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator+= (const Vector4<T>& other)
{
	mX += other.mX;
	mY += other.mY;
	mZ += other.mZ;
	mW += other.mW;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator-= (const Vector4<T>& other)
{
	mX -= other.mX;
	mY -= other.mY;
	mZ -= other.mZ;
	mW -= other.mW;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator*= (const Vector4<T>& other)
{
	mX *= other.mX;
	mY *= other.mY;
	mZ *= other.mZ;
	mW *= other.mW;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator/= (const Vector4<T>& other)
{
	mX /= other.mX;
	mY /= other.mY;
	mZ /= other.mZ;
	mW /= other.mW;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator+ (const T xyzw) const
{
	return Vector4<T>(mX + xyzw, mY + xyzw, mZ + xyzw, mW + xyzw);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator- (const T xyzw) const
{
	return Vector4<T>(mX - xyzw, mY - xyzw, mZ - xyzw, mW - xyzw);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator* (const T xyzw) const
{
	return Vector4<T>(mX * xyzw, mY * xyzw, mZ * xyzw, mW * xyzw);
}

template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator/ (const T xyzw) const
{
	return Vector4<T>(mX / xyzw, mY / xyzw, mZ / xyzw, mW / xyzw);
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator+= (const T xyzw)
{
	mX += xyzw;
	mY += xyzw;
	mZ += xyzw;
	mW += xyzw;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator-= (const T xyzw)
{
	mX -= xyzw;
	mY -= xyzw;
	mZ -= xyzw;
	mW -= xyzw;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator*= (const T xyzw)
{
	mX *= xyzw;
	mY *= xyzw;
	mZ *= xyzw;
	mW *= xyzw;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::operator/= (const T xyzw)
{
	mX /= xyzw;
	mY /= xyzw;
	mZ /= xyzw;
	mW /= xyzw;
	return *this;
}

template <typename T>
FORCEINLINE T Vector4<T>::DotProduct(const Vector4<T>& other) const
{
	return (mX * other.mX) + (mY * other.mY) + (mZ * other.mZ) + (mW * other.mW);
}

/*template <typename T>
FORCEINLINE Vector4<T>  Vector4<T>::CrossProduct(const Vector4<T>& other) const
{
	return Vector4<T>(m_Y * other.m_Z - m_Z * other.m_Y, m_Z * other.m_X - m_X * other.m_Z, m_X * other.m_Y - m_Y * other.m_X);
}*/

template <typename T>
FORCEINLINE T Vector4<T>::operator& (const Vector4<T>& other) const
{
	return DotProduct(other);
}

/*template <typename T>
FORCEINLINE Vector4<T> Vector4<T>::operator^ (const Vector4<T>& other) const
{
	return CrossProduct(other);
}*/

template <typename T>
FORCEINLINE T Vector4<T>::GetX() const
{
	return mX;
}

template <typename T>
FORCEINLINE T Vector4<T>::GetY() const
{
	return mY;
}
							 
template <typename T>		 
FORCEINLINE T Vector4<T>::GetZ() const
{ 
	return mZ;
}
							 
template <typename T>		 
FORCEINLINE T Vector4<T>::GetW() const
{ 
	return mW;
}

template <typename T>
FORCEINLINE void Vector4<T>::SetX(const T x)
{
	mX = x;
}

template <typename T>
FORCEINLINE void Vector4<T>::SetY(const T y)
{
	mY = y;
}

template <typename T>
FORCEINLINE void Vector4<T>::SetZ(const T z)
{
	mZ = z;
}

template <typename T>
FORCEINLINE void Vector4<T>::SetW(const T w)
{
	mW = w;
}

template <typename T>
FORCEINLINE void Vector4<T>::Set(const T x, const T y, const T z, const T w)
{
	mX = x;
	mY = y;
	mZ = z;
	mW = w;
}

template <typename T>
FORCEINLINE void Vector4<T>::Set(const T xyzw)
{
	mX = xyzw;
	mY = xyzw;
	mZ = xyzw;
	mW = xyzw;
}

template <typename T>
FORCEINLINE T Vector4<T>::Length() const
{
	return std::sqrt(*this & *this);
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::Normalize()
{
	T invLength = T(1) / Length();
	*this *= invLength;
	return *this;
}

template <typename T>
FORCEINLINE Vector4<T>& Vector4<T>::Reverse()
{
	std::reverse(mData);
	return *this;
}

template <typename T>
const Vector4<T> Vector4<T>::kZero = Vector4<T>(T(0), T(0), T(0), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kXAxis = Vector4<T>(T(1), T(0), T(0), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kYAxis = Vector4<T>(T(0), T(1), T(0), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kZAxis = Vector4<T>(T(0), T(0), T(1), T(0));

template <typename T>
const Vector4<T> Vector4<T>::kWAxis = Vector4<T>(T(0), T(0), T(0), T(1));

} // Math namespace
} // Pacman namespace
