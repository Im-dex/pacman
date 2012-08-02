#include <utility>
#include <algorithm>

namespace Pacman {
namespace Math {

template <typename T>
FORCEINLINE Vector2<T>::Vector2(const T x, const T y)
                      : mX(x),
                        mY(y)
{
}

template <typename T>
FORCEINLINE Vector2<T>::Vector2(const T xy)
                      : mX(xy),
                        mY(xy)
{
}

template <typename T>
FORCEINLINE Vector2<T>::Vector2(const T* data)
                      : mX(data[0]),
                        mY(data[1])
{
}

template <typename T>
FORCEINLINE Vector2<T>::Vector2(const Vector2<T>& other) 
                      : mData(other.mData)
{
}

template <typename T>
FORCEINLINE Vector2<T>::Vector2(Vector2<T>&& other)
{
	*this = std::move(other);
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator= (const Vector2<T>& other) 
{
    if (this != &other)
    {
    	mData = other.mData;
    }

    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator= (Vector2<T>&& other)
{
	if (this != &other)
	{
		std::swap(mData, other.mData);
	}

	return *this;
}

template <typename T>
FORCEINLINE bool Vector2<T>::operator== (const Vector2<T>& other) const
{
    return (Comparator<T>::Equals(mX, other.mX) && Comparator<T>::Equals(mY, other.mY));
}

template <typename T>
FORCEINLINE bool Vector2<T>::operator!= (const Vector2<T>& other) const
{
    return !(*this == other);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator+ (const Vector2<T>& other) const
{
    return Vector2<T>(mX + other.mX, mY + other.mY);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator- () const
{
    return Vector2<T>(-mX, -mY);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator- (const Vector2<T>& other) const
{
    return Vector2<T>(mX - other.mX, mY - other.mY);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator* (const Vector2<T>& other) const
{
    return Vector2<T>(mX * other.mX, mY * other.mY);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator/ (const Vector2<T>& other) const
{
    return Vector2<T>(mX / other.mX, mY / other.mY);
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator+= (const Vector2<T>& other)
{
    mX += other.mX;
    mY += other.mY;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator-= (const Vector2<T>& other)
{
    mX -= other.mX;
    mY -= other.mY;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator*= (const Vector2<T>& other)
{
    mX *= other.mX;
    mY *= other.mY;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator/= (const Vector2<T>& other)
{
    mX /= other.mX;
    mY /= other.mY;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator+ (const T xy) const
{
    return Vector2<T>(mX + xy, mY + xy);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator- (const T xy) const
{
    return Vector2<T>(mX - xy, mY - xy);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator* (const T xy) const
{
    return Vector2<T>(mX * xy, mY * xy);
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::operator/ (const T xy) const
{
    return Vector2<T>(mX / xy, mY / xy);
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator+= (const T xy)
{
    mX += xy;
    mY += xy;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator-= (const T xy)
{
    mX -= xy;
    mY -= xy;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator*= (const T xy)
{
    mX *= xy;
    mY *= xy;
    return *this;
}

template <typename T>
FORCEINLINE Vector2<T>& Vector2<T>::operator/= (const T xy)
{
    mX /= xy;
    mY /= xy;
    return *this;
}

template <typename T>
FORCEINLINE T Vector2<T>::DotProduct(const Vector2<T>& other) const
{
    return (mX * other.mX) + (mY * other.mY);
}

template <typename T>
FORCEINLINE T Vector2<T>::operator& (const Vector2<T>& other) const
{
    return DotProduct(other);
}

template <typename T>
T Vector2<T>::Length() const
{
    return std::sqrt((mX * mX) + (mY * mY));
}

template <typename T>
FORCEINLINE T Vector2<T>::GetX() const
{ 
    return mX;
}
                              
template <typename T>		  
FORCEINLINE T Vector2<T>::GetY() const
{ 
    return mY;
}

template <typename T>
FORCEINLINE void Vector2<T>::SetX(const T x)
{
    mX = x;
}

template <typename T>
FORCEINLINE void Vector2<T>::SetY(const T y)
{
    mY = y;
}

template <typename T>
FORCEINLINE Vector2<T> Vector2<T>::Reverse() const
{
    return Vector2<T>(mY, mX);
}

template <typename T>
const Vector2<T> Vector2<T>::kZero = Vector2<T>(T(0), T(0));

template <typename T>
const Vector2<T> Vector2<T>::kXAxis = Vector2<T>(T(1), T(0));

template <typename T>
const Vector2<T> Vector2<T>::kYAxis = Vector2<T>(T(0), T(1));

} // Math namespace
} // Pacman namespace
