#include "jni_utility.h"

namespace Pacman {
namespace JNI {

static JavaVM* gJavaVM;

extern "C" {
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	gJavaVM = vm;
	return JNI_VERSION_1_6;
}

//================================================================================================================================

MethodInfo::MethodInfo(const jclass cls, const jmethodID mid)
		  : methodClass(cls),
			methodId(mid)
{
}

MethodInfo FindStaticMethod(JNIEnv* env, const char* className, const char* methodName, const char* methodSignature)
{
	jclass cls = env->FindClass(className);
	PACMAN_CHECK_ERROR(cls != nullptr, ErrorCode::JClassNotFound);

	jmethodID mid = env->GetStaticMethodID(cls, methodName, methodSignature);
	PACMAN_CHECK_ERROR(mid != nullptr, ErrorCode::JFuncNotFound);

	return MethodInfo(cls, mid);
}

JNIEnv* GetEnv()
{
	void* env = nullptr;
	jint res = gJavaVM->GetEnv(&env, JNI_VERSION_1_6);
	PACMAN_CHECK_ERROR(res >= 0, ErrorCode::JNICallFailed);
	return static_cast<JNIEnv*>(env);
}

jstring MakeUTF8String(const char* string)
{
	return GetEnv()->NewStringUTF(string);
}

} // JNI namespace
} // Pacman namespace