#pragma once

#include <jni.h>

#include "base.h"
#include "error.h"

namespace Pacman {
namespace JNI {

struct MethodInfo
{
	MethodInfo() = delete;
	MethodInfo(const jclass cls, const jmethodID mid);
	MethodInfo(const MethodInfo&) = default;
	~MethodInfo() = default;

	MethodInfo& operator= (const MethodInfo&) = default;

	jclass methodClass;
	jmethodID methodId;
};

JNIEnv* GetEnv();

jstring MakeUTF8String(const char* string);

MethodInfo FindStaticMethod(JNIEnv* env, const char* className, const char* methodName, const char* methodSignature);

template <typename... Args>
void CallStaticVoidMethod(const char* className, const char* methodName, const char* methodSignature, Args... args)
{
	JNIEnv* env = GetEnv();
	MethodInfo info = FindStaticMethod(env, className, methodName, methodSignature);
	env->CallStaticVoidMethod(info.methodClass, info.methodId, args...);
}

template <typename... Args>
void CallStaticVoidMethod(const MethodInfo& info, Args... args)
{
	GetEnv()->CallStaticVoidMethod(info.methodClass, info.methodId, args...);
}

template <typename... Args>
jobject CallStaticObjectMethod(const char* className, const char* methodName, const char* methodSignature, Args... args)
{
	JNIEnv* env = GetEnv();
	MethodInfo info = FindStaticMethod(env, className, methodName, methodSignature);
	return env->CallStaticObjectMethod(info.methodClass, info.methodId, args...);
}

template <typename... Args>
jobject CallStaticObjectMethod(const MethodInfo& info, Args... args)
{
	return GetEnv()->CallStaticObjectMethod(info.methodClass, info.methodId, args...);
}

} // JNI namespace
} // Pacman namespace