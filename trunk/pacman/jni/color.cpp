#include "color.h"

#include <utility>
#include <algorithm>

namespace Pacman {

const Color Color::kRed   = Color(0xff, 0x00, 0x00, 0xff);
const Color Color::kGreen = Color(0x00,	0xff, 0x00, 0xff);
const Color Color::kBlue  = Color(0x00,	0x00, 0xff, 0xff);
const Color Color::kWhite = Color(0xff, 0xff, 0xff, 0xff);
const Color Color::kBlack = Color(0x00,	0x00, 0x00, 0xff);

Color::Color(const byte_t r, const byte_t g, const byte_t b, const byte_t a)
	 : mR(r),
	   mG(g),
	   mB(b),
	   mA(a)
{
}

Color::Color(Color&& other)
{
	*this = std::move(other);
}

Color& Color::operator= (Color&& other)
{
	std::swap(mData, other.mData);
	return *this;
}

bool Color::operator== (const Color& other) const
{
	return (mR == other.mR) && (mG == other.mG) && (mB == other.mB) && (mA == other.mA);
}

bool Color::operator!= (const Color& other) const
{
	return !(*this == other);
}

void Color::Fill(byte_t* buffer, const size_t componentsCount) const
{
	for (byte_t component : mData)
	{
		*buffer = component;
		buffer++;
	}
}

} // Pacman namespace
