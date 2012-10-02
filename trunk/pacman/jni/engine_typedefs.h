#pragma once

#include "base.h"
#include "rect.h"
#include "math/vector3.h"

namespace Pacman {

typedef Rect<float>             TextureRegion;
typedef Rect<Size>              SpriteRegion;
typedef SpriteRegion::Position  Position;
typedef Math::Vector3f          Rotation;
typedef SizeOffset              PosOffset;

} // Pacman namespace