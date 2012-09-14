#pragma once

#include "base.h"
#include "rect.h"
#include "math/vector3.h"

namespace Pacman {

typedef Rect<float> TextureRegion;
typedef Rect<uint16_t> SpriteRegion;
typedef SpriteRegion::Position SpritePosition;
typedef Math::Vector3f Rotation;

} // Pacman namespace