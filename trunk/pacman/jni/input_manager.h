#pragma once

#include <memory>
#include <atomic>

#include "base.h"
#include "utils.h"
#include "engine_listeners.h"

namespace Pacman {

enum class TouchEvent
{
    Up   = 0,
    Down = 1,
    Move = 2
};

struct TouchInfo
{
    TouchEvent event;
    float x;
    float y;
};

class InputManager
{
public:

    InputManager();
    InputManager(const InputManager&) = delete;
    ~InputManager() = default;

    InputManager& operator= (const InputManager&) = delete;

    void PushInfo(const TouchInfo& touchInfo);

    void Update();

    void SetListener(const std::shared_ptr<IGestureListener>& listener)
    {
        mListener = listener;
    }

private:

    typedef EnumType<GestureType>::value GestureEnumType;

    TouchInfo                         mBeginGestureTouch;
    std::atomic<GestureEnumType>      mLastGesture;
    std::shared_ptr<IGestureListener> mListener;
};

} // Pacman namespace