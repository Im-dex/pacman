#pragma once

#include <cstdint>

#include "base.h"

namespace Pacman {

class Engine;

class IEngineListener
{
public:

	virtual void OnStart(Engine& engine) = 0;

	virtual void OnStop(Engine& engine) = 0;

	virtual void OnUpdate(Engine& engine, const uint64_t dt) = 0;
};

enum class GestureType : uint8_t
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