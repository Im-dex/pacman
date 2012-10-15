#pragma once

#include <android/log.h>
#include <utility>

#include "base.h"

namespace Pacman {

static const char* kTag = "NativeLib";

template <typename... Args>
static FORCEINLINE void LogI(Args&&... args)
{
    __android_log_print(ANDROID_LOG_INFO,  kTag, std::forward<Args>(args)...);
}

template <typename... Args>
static FORCEINLINE void LogE(Args&&... args)
{
    __android_log_print(ANDROID_LOG_ERROR,  kTag, std::forward<Args>(args)...);
}

} // Pacman namespace