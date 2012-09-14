#include "main.h"

#include <jni.h>
#include <cassert>

#include "log.h"
#include "error.h"
#include "engine.h"

namespace Pacman {

static Engine* gEngine = nullptr;

Engine* GetEngine()
{
	assert(gEngine != nullptr);
	return gEngine;
}

void Init()
{
	gEngine = new Engine();
	OnPacmanApplicationStart();
}

void DeInit()
{
	OnPacmanApplicationEnd();
	delete gEngine;
	gEngine = nullptr;
}

void ResizeViewport(const size_t width, const size_t heigth)
{
	LOGI("w: %d, h: %d", width, heigth);
	gEngine->Init(width, heigth);
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

void StdExceptionCatched(std::exception& e)
{
	LOGE("Exception has been catched: %s", e.what());
	ErrorHandler::Terminate();
}

void UnknownExceptionCatched()
{
	LOGE("Unknown exception has been catched");
	ErrorHandler::Terminate();
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_init(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_deinit(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_resizeViewport(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_drawFrame(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_touchEvent(JNIEnv * env, jobject obj, jint event, jfloat x, jfloat y);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_init(JNIEnv* env, jobject obj)
{
	try
	{
		Init();
	}
	catch (std::exception& e)
	{
		StdExceptionCatched(e);
	}
	catch(...)
	{
		UnknownExceptionCatched();
	}
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_deinit(JNIEnv* env, jobject obj)
{
	try
	{
		DeInit();
	}
	catch (std::exception& e)
	{
		StdExceptionCatched(e);
	}
	catch(...)
	{
		UnknownExceptionCatched();
	}
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_resizeViewport(JNIEnv* env, jobject obj, jint width, jint heigth)
{
	try
	{
		ResizeViewport(width, heigth);
	}
	catch (std::exception& e)
	{
		StdExceptionCatched(e);
	}
	catch(...)
	{
		UnknownExceptionCatched();
	}
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_drawFrame(JNIEnv* env, jobject obj)
{
	try
	{
		DrawFrame();
	}
	catch (std::exception& e)
	{
		StdExceptionCatched(e);
	}
	catch(...)
	{
		UnknownExceptionCatched();
	}
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_touchEvent(JNIEnv * env, jobject obj, jint event, jfloat x, jfloat y)
{
	try
	{
		TouchEvent(event, x, y);
	}
	catch (std::exception& e)
	{
		StdExceptionCatched(e);
	}
	catch(...)
	{
		UnknownExceptionCatched();
	}

}

} // Pacman namespace
