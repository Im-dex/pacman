#pragma once

#include <cstdint>

#include "base.h"

namespace Pacman {

class IEngineListener
{
public:

	virtual void OnLoad() = 0;

	virtual void OnUnload() = 0;

	virtual void OnUpdate(const uint64_t dt) = 0;
};

enum class GestureType
{
    None,
    LeftSwipe,
    RightSwipe,
    TopSwipe,
    BottomSwipe
};

class IGestureListener
{
public:

    virtual void OnGesture(const GestureType gestureType) = 0;
};

} // Pacman namespace