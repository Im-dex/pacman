#pragma once

#include "base.h"
#include "engine_forwdecl.h"

namespace Pacman {

class IEngineListener
{
public:

	virtual void OnStart(const Engine& engine) = 0;

	virtual void OnStop(const Engine& engine) = 0;

	virtual void OnUpdate(const Engine& engine, const uint64_t dt) = 0;
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