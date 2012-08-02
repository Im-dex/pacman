#include <cmath>
#include <algorithm>
#include <utility>

namespace Pacman {
namespace Math {

template <typename T>
FORCEINLINE Vector3<T>::Vector3(const T x, const T y, const T z)
					  : mX(x),
					    mY(y),
						mZ(z)
{
}

template <typename T>
FORCEINLINE Vector3<T>::Vector3(const T xyz)
					  : mX(xyz),
					    mY(xyz),
						mZ(xyz)
{
}

template <typename T>
FORCEINLINE Vector3<T>::Vector3(const T* data)
					  : mX(data[0]),
					    mY(data[1]),
						mZ(data[2])
{
}

template <typename T>
FORCEINLINE Vector3<T>::Vector3(const Vector3<T>& other) 
					  : mData(other.mData)
{
}

template <typename T>
FORCEINLINE Vector3<T>::Vector3(Vector3<T>&& other) 
{ 
	*this = std::move(other);
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator= (const Vector3<T>& other) 
{
	if (this != &other)
	{
		mData = other.mData;
	}

	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator= (Vector3<T>&& other)
{
	if (this != &other)
	{
		std::swap(mData, other.mData);
	}

	return *this;
}

template <typename T>
FORCEINLINE bool Vector3<T>::operator== (const Vector3<T>& other) const
{
	return (Comparator<T>::Equals(mX, other.mX) && Comparator<T>::Equals(mY, other.mY) && Comparator<T>::Equals(mZ, other.mZ));
}

template <typename T>
FORCEINLINE bool Vector3<T>::operator!= (const Vector3<T>& other) const
{
	return !(*this == other);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator+ (const Vector3<T>& other) const
{
	return Vector3<T>(mX + other.mX, mY + other.mY, mZ + other.mZ);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator- () const
{
	return Vector3<T>(-mX, -mY, -mZ);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator- (const Vector3<T>& other) const
{
	return Vector3<T>(mX - other.mX, mY - other.mY, mZ - other.mZ);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator* (const Vector3<T>& other) const
{
	return Vector3<T>(mX * other.mX, mY * other.mY, mZ * other.mZ);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator/ (const Vector3<T>& other) const
{
	return Vector3<T>(mX / other.mX, mY / other.mY, mZ / other.mZ);
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator+= (const Vector3<T>& other)
{
	mX += other.mX;
	mY += other.mY;
	mZ += other.mZ;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator-= (const Vector3<T>& other)
{
	mX -= other.mX;
	mY -= other.mY;
	mZ -= other.mZ;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator*= (const Vector3<T>& other)
{
	mX *= other.mX;
	mY *= other.mY;
	mZ *= other.mZ;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator/= (const Vector3<T>& other)
{
	mX /= other.mX;
	mY /= other.mY;
	mZ /= other.mZ;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator+ (const T xyz) const
{
	return Vector3<T>(mX + xyz, mY + xyz, mZ + xyz);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator- (const T xyz) const
{
	return Vector3<T>(mX - xyz, mY - xyz, mZ - xyz);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator* (const T xyz) const
{
	return Vector3<T>(mX * xyz, mY * xyz, mZ * xyz);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator/ (const T xyz) const
{
	return Vector3<T>(mX / xyz, mY / xyz, mZ / xyz);
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator+= (const T xyz)
{
	mX += xyz;
	mY += xyz;
	mZ += xyz;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator-= (const T xyz)
{
	mX -= xyz;
	mY -= xyz;
	mZ -= xyz;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator*= (const T xyz)
{
	mX *= xyz;
	mY *= xyz;
	mZ *= xyz;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::operator/= (const T xyz)
{
	mX /= xyz;
	mY /= xyz;
	mZ /= xyz;
	return *this;
}

template <typename T>
FORCEINLINE T Vector3<T>::DotProduct(const Vector3<T>& other) const
{
	return (mX * other.mX) + (mY * other.mY) + (mZ * other.mZ);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::CrossProduct(const Vector3<T>& other) const
{
	return Vector3<T>(mY * other.mZ - mZ * other.mY, mZ * other.mX - mX * other.mZ, mX * other.mY - mY * other.mX);
}

template <typename T>
FORCEINLINE T Vector3<T>::operator& (const Vector3<T>& other) const
{
	return DotProduct(other);
}

template <typename T>
FORCEINLINE Vector3<T> Vector3<T>::operator^ (const Vector3<T>& other) const
{
	return CrossProduct(other);
}

template <typename T>
FORCEINLINE T Vector3<T>::GetX() const
{
	return mX;
}

template <typename T>
FORCEINLINE T Vector3<T>::GetY() const
{ 
	return mY;
}

template <typename T>
FORCEINLINE T Vector3<T>::GetZ() const
{ 
	return mZ;
}

template <typename T>
FORCEINLINE void Vector3<T>::SetX(const T x)
{
	mX = x;
}

template <typename T>
FORCEINLINE void Vector3<T>::SetY(const T y)
{
	mY = y;
}

template <typename T>
FORCEINLINE void Vector3<T>::SetZ(const T z)
{
	mZ = z;
}

template <typename T>
FORCEINLINE void Vector3<T>::Set(const T x, const T y, const T z)
{
	mX = x;
	mY = y;
	mZ = z;
}

template <typename T>
FORCEINLINE void Vector3<T>::Set(const T xyz)
{
	mX = xyz;
	mY = xyz;
	mZ = xyz;
}

template <typename T>
FORCEINLINE T Vector3<T>::Length() const
{
	return std::sqrt((mX * mX) + (mY * mY) + (mZ * mZ));
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::Normalize()
{
	T invLength = T(1) / Length();
	*this *= invLength;
	return *this;
}

template <typename T>
FORCEINLINE Vector3<T>& Vector3<T>::Reverse()
{
	std::reverse(mData);
	return *this;
}

template <typename T>
const Vector3<T> Vector3<T>::kZero = Vector3<T>(T(0), T(0), T(0));

template <typename T>
const Vector3<T> Vector3<T>::kXAxis = Vector3<T>(T(1), T(0), T(0));

template <typename T>
const Vector3<T> Vector3<T>::kYAxis = Vector3<T>(T(0), T(1), T(0));

template <typename T>
const Vector3<T> Vector3<T>::kZAxis = Vector3<T>(T(0), T(0), T(1));

} // Math namespace
} // Pacman namespace
