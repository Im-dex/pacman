#pragma once

#include "base.h"

#include <utility>
#include <algorithm>
#include <type_traits>

namespace Pacman {

template <typename T>
class Rect
{
public:

	static_assert(std::is_arithmetic<T>::value, "Wrong type");

	Rect() = delete;
	Rect(const T x, const T y, const T width, const T height);
	Rect(const Rect& other);
	Rect(Rect&& other);
	~Rect() = default;

	Rect& operator= (const Rect& other);
	Rect& operator= (Rect&& other);

	void Scale(const float factor);

	T GetX() const
	{
		return mX;
	}

	T GetY() const
	{
		return mY;
	}

	T GetWidth() const
	{
		return mWidth;
	}

	T GetHeight() const
	{
		return mHeight;
	}

	void SetX(const T x)
	{
		mX = x;
	}

	void SetY(const T y)
	{
		mY = y;
	}

	void SetWidth(const T width)
	{
		mWidth = width;
	}

	void SetHeight(const T height)
	{
		mHeight = height;
	}

private:

	T mX;
	T mY;
	T mWidth;
	T mHeight;
};

template <typename T>
Rect<T>::Rect(const T x, const T y, const T width, const T height)
	   : mX(x),
		 mY(y),
		 mWidth(width),
		 mHeight(height)
{
}

template <typename T>
Rect<T>::Rect(const Rect& other)
	   : mX(other.mX),
		 mY(other.mY),
	     mWidth(other.mWidth),
		 mHeight(other.mHeight)
{
}

template <typename T>
Rect<T>::Rect(Rect&& other)
{
	*this = std::move(other);
}

template <typename T>
FORCEINLINE Rect<T>& Rect<T>::operator= (const Rect& other)
{
	if (this != &other)
	{
		mX = other.mX;
		mY = other.mY;
		mWidth = other.mWidth;
		mHeight = other.mHeight;
	}
	
	return *this;
}

template <typename T>
FORCEINLINE Rect<T>& Rect<T>::operator= (Rect&& other)
{
	if (this != &other)
	{
		std::swap(mX, other.mX);
		std::swap(mY, other.mY);
		std::swap(mWidth, other.mWidth);
		std::swap(mHeight, other.mHeight);
	}

	return *this;
}

template <typename T>
FORCEINLINE void Rect<T>::Scale(const float factor)
{
	mX = static_cast<T>(mX * factor);
	mY = static_cast<T>(mY * factor);
	mWidth = static_cast<T>(mWidth * factor);
	mHeight = static_cast<T>(mHeight * factor);
}

} // Pacmannamespace