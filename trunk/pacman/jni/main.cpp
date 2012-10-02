#include "main.h"

#include <jni.h>
#include <memory>

#include "log.h"
#include "error.h"
#include "engine.h"

#define JNI_CALLBACK_CALL(functionName, ...)\
    try\
    {\
        functionName(__VA_ARGS__);\
    }\
    catch (const std::exception& e)\
    {\
        StdExceptionCatched(e);\
    }\
    catch(...)\
    {\
        UnknownExceptionCatched();\
    }

namespace Pacman {

static std::unique_ptr<Engine> gEngine = nullptr;

Engine& GetEngine()
{
	return *(gEngine.get());
}

void Start()
{
	gEngine = std::unique_ptr<Engine>(new Engine());
    PacmanSetEngineListener(GetEngine());
}

void Stop()
{
    gEngine->Stop();
	gEngine = nullptr;
}

void Pause()
{
    gEngine->Pause();
}

void Resume()
{
    gEngine->Resume();
}

void SurfaceChanged(const size_t width, const size_t heigth)
{
    if (!gEngine->IsStarted())
    {
	    LogI("w: %d, h: %d", width, heigth);
	    gEngine->Start(width, heigth);
    }
}

void DrawFrame()
{
	gEngine->OnDrawFrame();
}

void TouchEvent(const int event, const float x, const float y)
{
	gEngine->OnTouch(event, x, y);
}

//========================================================================================================================

void StdExceptionCatched(const std::exception& e)
{
	LogE("Exception has been catched: %s", e.what());
	ErrorHandler::Terminate();
}

void UnknownExceptionCatched()
{
	LogE("Unknown exception has been catched");
	ErrorHandler::Terminate();
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_start(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_stop(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_pause(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_resume(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_surfaceChanged(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_drawFrame(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_touchEvent(JNIEnv * env, jobject obj, jint event, jfloat x, jfloat y);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_start(JNIEnv* env, jobject obj)
{
    JNI_CALLBACK_CALL(Start);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_stop(JNIEnv* env, jobject obj)
{
    JNI_CALLBACK_CALL(Stop);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_pause(JNIEnv* env, jobject obj)
{
    JNI_CALLBACK_CALL(Pause);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_resume(JNIEnv* env, jobject obj)
{
    JNI_CALLBACK_CALL(Resume);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_surfaceChanged(JNIEnv* env, jobject obj, jint width, jint heigth)
{
    JNI_CALLBACK_CALL(SurfaceChanged, width, heigth);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_drawFrame(JNIEnv* env, jobject obj)
{
    JNI_CALLBACK_CALL(DrawFrame);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_touchEvent(JNIEnv * env, jobject obj, jint event, jfloat x, jfloat y)
{
    JNI_CALLBACK_CALL(TouchEvent, event, x, y);
}

} // Pacman namespace
