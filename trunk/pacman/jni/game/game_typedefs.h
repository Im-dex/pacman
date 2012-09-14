#pragma once

#include <cstdint>

#include "math/vector2.h"

namespace Pacman {

typedef Math::Vector2<uint16_t> CellIndex;

enum class DotType : uint8_t
{
    None  = 0,
    Small = 4,
    Big   = 5
};

static const size_t kDotTypesCount = 2;

} // Pacman namespace