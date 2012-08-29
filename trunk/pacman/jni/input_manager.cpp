#include "input_manager.h"

#include <complex>

namespace Pacman {

static GestureType ConvertToGesture(const TouchInfo& begin, const TouchInfo& end)
{
    const float xDiff = std::abs(end.x - begin.x);
    const float yDiff = std::abs(end.y - begin.y);

    if (xDiff > yDiff)
    {
        if (end.x > begin.x)
            return GestureType::RightSwipe;
        else
            return GestureType::LeftSwipe;
    }
    else
    {
        if (end.y > begin.y)
            return GestureType::BottomSwipe;
        else
            return GestureType::TopSwipe;
    }
}

InputManager::InputManager()
            : mMutex(),
              mLastGesture(GestureType::None),
              mListener(nullptr)
{
    mBeginGestureTouch = { TouchEvent::Move, 0.0f, 0.0f };
}

void InputManager::PushInfo(const TouchInfo& touchInfo)
{
    if (touchInfo.event == TouchEvent::Move)
        return;

    if (touchInfo.event == TouchEvent::Down)
    {
        mBeginGestureTouch = touchInfo;
    }
    else
    {
        mMutex.Lock();
        mLastGesture = ConvertToGesture(mBeginGestureTouch, touchInfo);
        mMutex.Unlock();
    }
}

void InputManager::Update()
{
    mMutex.Lock();
    if (mLastGesture != GestureType::None && mListener != nullptr)
    {
        mListener->OnGesture(mLastGesture);
        mLastGesture = GestureType::None;
    }
    mMutex.Unlock();
}

} // Pacman namespace