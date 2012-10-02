#pragma once

#include <android/log.h>

#include "base.h"

#define LOG_TAG "NativeLib"
static const char* kTag = "NativeLib";

template <typename... Args>
static FORCEINLINE void LogI(const Args&... args)
{
    __android_log_print(ANDROID_LOG_INFO,  kTag, args...);
}

template <typename... Args>
static FORCEINLINE void LogE(const Args&... args)
{
    __android_log_print(ANDROID_LOG_ERROR,  kTag, args...);
}