#pragma once

#include "base.h"

namespace Pacman {

class Color
{
public:

	static const Color kRed;
	static const Color kGreen;
	static const Color kBlue;
	static const Color kWhite;
	static const Color kBlack;

	Color() = delete;
	Color(const byte_t r, const byte_t g, const byte_t b, const byte_t a);
	Color(const Color& other) = default;
	~Color() = default;

	Color& operator= (const Color& other) = default;

	bool operator== (const Color& other) const;
	bool operator!= (const Color& other) const;

	byte_t GetRed() const
	{
		return mR;
	}

	float GetRedFloat() const
	{
		return ConvertToFloat(mR);
	}

	byte_t GetGreen() const
	{
		return mG;
	}

	float GetGreenFloat() const
	{
		return ConvertToFloat(mG);
	}

	byte_t GetBlue() const
	{
		return mB;
	}

	float GetBlueFloat() const
	{
		return ConvertToFloat(mB);
	}

	byte_t GetAlpha() const
	{
		return mA;
	}

	float GetAlphaFloat() const
	{
		return ConvertToFloat(mA);
	}

	void SetRed(const byte_t r)
	{
		mR = r;
	}

	void SetGreen(const byte_t g)
	{
		mG = g;
	}

	void SetBlue(const byte_t b)
	{
		mB = b;
	}

	void SetAlpha(const byte_t a)
	{
		mA = a;
	}

	void SetRGBA(const byte_t r, const byte_t g, const byte_t b, const byte_t a)
	{
		mR = r;
		mG = g;
		mB = b;
		mA = a;
	}

private:

	float ConvertToFloat(const byte_t value) const
	{
		return static_cast<float>(value) * (1.0f / 255.0f);
	}

	byte_t mR;
	byte_t mG;
	byte_t mB;
	byte_t mA;
};

} // Pacman namespace
