#pragma once

#include "mutex.h"
#include "engine_listeners.h"

#include <memory>

namespace Pacman {

enum class TouchEvent
{
    Up   = 0,
    Down = 1,
    Move = 2
};

static const size_t kTouchEventsCount = 3;

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

    void SetListener(std::shared_ptr<IGestureListener> listener)
    {
        mListener = listener;
    }

private:

    Mutex                             mMutex;
    TouchInfo                         mBeginGestureTouch;
    GestureType                       mLastGesture;
    std::shared_ptr<IGestureListener> mListener;
};

} // Pacman namespace