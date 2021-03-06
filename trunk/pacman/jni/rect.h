#pragma once

#include <utility>
#include <algorithm>
#include <type_traits>

#include "base.h"
#include "math/vector2.h"

namespace Pacman {

template <typename T>
class Rect
{
public:

	static_assert(std::is_arithmetic<T>::value, "Wrong type");

    typedef T value_t;
	typedef Math::Vector2<T> Position;

    static const Rect<T> kZero;

	Rect() = delete;
	// x, y - position
	Rect(const T x, const T y, const T width, const T height);
	// position - the left top point position
	Rect(const Position position, const T width, const T height);
	Rect(const Rect<T>& other);
	~Rect() = default;

	Rect& operator= (const Rect<T>& other);

	Position GetPosition() const;
	T GetPosX() const;
	T GetPosY() const;
	T GetWidth() const;
	T GetHeight() const;

	Position GetLeftBottomPos() const;
	Position GetRightTopPos() const;
	Position GetRightBottomPos() const;
    Position GetCenterPos() const;

	T GetLeftBottomPosX() const;
	T GetLeftBottomPosY() const;
	T GetRightTopPosX() const;
	T GetRightTopPosY() const;
	T GetRightBottomPosX() const;
	T GetRightBottomPosY() const;
    T GetCenterPosX() const;
    T GetCenterPosY() const;

	void SetPosition(Position leftTop);
	void SetPosX(const T x);
	void SetPosY(const T y);
	void SetWidth(const T width);
	void SetHeight(const T height);

    bool HasPoint(const T x, const T y) const;
    bool HasPoint(const Position& point) const;

    bool IsIntersect(const Rect<T>& other) const;
    Rect<T> Intersection(const Rect<T>& other) const;

private:

	Position mPosition;
	T mWidth;
	T mHeight;
};

template <typename T>
const Rect<T> Rect<T>::kZero = Rect<T>(T(0), T(0), T(0), T(0));

template <typename T>
Rect<T>::Rect(const T x, const T y, const T width, const T height)
	: mPosition(x, y),
	  mWidth(width),
	  mHeight(height)
{
}

template <typename T>
Rect<T>::Rect(const Rect<T>::Position position, const T width, const T height)
	   : mPosition(position),
		 mWidth(width),
		 mHeight(height)
{
}

template <typename T>
Rect<T>::Rect(const Rect<T>& other)
	   : mPosition(other.mPosition),
	     mWidth(other.mWidth),
		 mHeight(other.mHeight)
{
}

template <typename T>
FORCEINLINE Rect<T>& Rect<T>::operator= (const Rect<T>& other)
{
	if (this != &other)
	{
		mPosition = other.mPosition;
		mWidth = other.mWidth;
		mHeight = other.mHeight;
	}
	
	return *this;
}

template <typename T>
FORCEINLINE typename Rect<T>::Position Rect<T>::GetPosition() const
{
	return mPosition;
}

template <typename T>
FORCEINLINE T Rect<T>::GetPosX() const
{
	return mPosition.GetX();
}

template <typename T>
FORCEINLINE T Rect<T>::GetPosY() const
{
	return mPosition.GetY();
}

template <typename T>
FORCEINLINE T Rect<T>::GetWidth() const
{
	return mWidth;
}

template <typename T>
FORCEINLINE T Rect<T>::GetHeight() const
{
	return mHeight;
}

template <typename T>
FORCEINLINE typename Rect<T>::Position Rect<T>::GetLeftBottomPos() const
{
	return Math::Vector2<T>(mPosition.GetX(), mPosition.GetY() + mHeight);
}

template <typename T>
FORCEINLINE typename Rect<T>::Position Rect<T>::GetRightTopPos() const
{
	return Math::Vector2<T>(mPosition.GetX() + mWidth, mPosition.GetY());
}

template <typename T>
FORCEINLINE typename Rect<T>::Position Rect<T>::GetRightBottomPos() const
{
	return Math::Vector2<T>(mPosition.GetX() + mWidth, mPosition.GetY() + mHeight);
}

template <typename T>
FORCEINLINE typename Rect<T>::Position Rect<T>::GetCenterPos() const
{
    return Math::Vector2<T>(mPosition.GetX() + mWidth / 2, mPosition.GetY() + mHeight / 2);
}

template <typename T>
FORCEINLINE T Rect<T>::GetLeftBottomPosX() const
{
	return GetLeftBottomPos().GetX();
}

template <typename T>
FORCEINLINE T Rect<T>::GetLeftBottomPosY() const
{
	return GetLeftBottomPos().GetY();
}

template <typename T>
FORCEINLINE T Rect<T>::GetRightTopPosX() const
{
	return GetRightTopPos().GetX();
}

template <typename T>
FORCEINLINE T Rect<T>::GetRightTopPosY() const
{
	return GetRightTopPos().GetY();
}

template <typename T>
FORCEINLINE T Rect<T>::GetRightBottomPosX() const
{
	return GetRightBottomPos().GetX();
}

template <typename T>
FORCEINLINE T Rect<T>::GetRightBottomPosY() const
{
	return GetRightBottomPos().GetY();
}

template <typename T>
FORCEINLINE T Rect<T>::GetCenterPosX() const
{
    return GetCenterPos().GetX();
}

template <typename T>
FORCEINLINE T Rect<T>::GetCenterPosY() const
{
    return GetCenterPos().GetY();
}

template <typename T>
FORCEINLINE void Rect<T>::SetPosition(const Rect<T>::Position position)
{
	mPosition = position;
}

template <typename T>
FORCEINLINE void Rect<T>::SetPosX(const T x)
{
	return mPosition.SetX(x);
}

template <typename T>
FORCEINLINE void Rect<T>::SetPosY(const T y)
{
	return mPosition.SetY(y);
}

template <typename T>
FORCEINLINE void Rect<T>::SetWidth(const T width)
{
	mWidth = width;
}

template <typename T>
FORCEINLINE void Rect<T>::SetHeight(const T height)
{
	mHeight = height;
}

template <typename T>
FORCEINLINE bool Rect<T>::HasPoint(const T x, const T y) const
{
    return (x >= mPosition.GetX()) && (x <= mPosition.GetX() + mWidth) &&
           (y >= mPosition.GetY()) && (y <= mPosition.GetY() + mHeight);
}

template <typename T>
FORCEINLINE bool Rect<T>::HasPoint(const Position& point) const
{
    return HasPoint(point.GetX(), point.GetY());
}

template <typename T>
FORCEINLINE bool Rect<T>::IsIntersect(const Rect<T>& other) const
{
    const Position thisRightBottomPos = GetRightBottomPos();
    const Position otherRightBottomPos = other.GetRightBottomPos();
    return (thisRightBottomPos.GetX() >= other.GetPosX()) && (GetPosX() <= otherRightBottomPos.GetX()) &&
           (thisRightBottomPos.GetY() >= other.GetPosY()) && (GetPosY() <= otherRightBottomPos.GetY());
}

template <typename T>
FORCEINLINE Rect<T> Rect<T>::Intersection(const Rect<T>& other) const
{
    const T leftX = std::max(GetPosX(), other.GetPosX());
    const T rightX = std::min(GetRightBottomPosX(), other.GetRightBottomPosX());
    const T topY = std::max(GetPosY(), other.GetPosY());
    const T bottomY = std::min(GetRightBottomPosY(), other.GetRightBottomPosY());
    return Rect<T>(leftX, topY, rightX - leftX, bottomY - topY);
}

} // Pacmannamespace