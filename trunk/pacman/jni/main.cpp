#include <jni.h>

#include "base.h"
#include "error.h"
#include "renderer.h"
#include "scene_manager.h"

#include "asset_manager.h"
#include "sprite.h"
#include "shader_program.h"

#include <memory>

namespace Pacman {

Renderer* renderer = nullptr;
std::shared_ptr<SceneManager> sceneManager = nullptr;
Sprite* testSprite = nullptr;
std::shared_ptr<ShaderProgram> shaderProgram;

static const char kVertexShader[] = "attribute vec4 vPosition;"
									"attribute vec4 vColor;\n"
									"//uniform mat4 mProjectionMatrix;\n"
									"//uniform mat4 mModelMatrix;\n"
									"uniform mat4 mModelProjectionMatrix;\n"
									"varying vec4 vVertColor;\n"
									"void main() {\n"
									"	vVertColor = vColor;\n"
									"	gl_Position = mModelProjectionMatrix * vPosition;\n"
									"}\n";

static const char kFragmentShader[] = "precision mediump float;\n"
							  	  	  "varying vec4 vVertColor;\n"
							  	  	  "void main() {\n"
							  	  	  "	gl_FragColor = vVertColor;\n"
							  	  	  "}\n";

void Init(JNIEnv* env)
{
	renderer = new Renderer();
	//AssetManager::LoadTexture(env, "part.png", TextureFiltering::None, TextureRepeat::None);
}

void DeInit()
{
	shaderProgram = nullptr;
	delete testSprite;
	sceneManager = nullptr;
	delete renderer;
}

void ResizeViewport(const size_t width, const size_t heigth)
{
	LOGI("w: %d, h: %d", width, heigth);
	renderer->Init(width, heigth);
	sceneManager = std::make_shared<SceneManager>(static_cast<const float>(width), static_cast<const float>(heigth));
	renderer->SetSceneManager(sceneManager);

	shaderProgram = std::make_shared<ShaderProgram>(kVertexShader, kFragmentShader);
	shaderProgram->Link();

	testSprite = new Sprite(100.0f, 100.0f, Color::kRed, Color::kGreen, Color::kBlue, Color::kRed, shaderProgram);

	SceneNode node(*testSprite, Math::Vector2f(100.0f, 50.0f));
	sceneManager->AttachNode(node);
}

void DrawFrame()
{
	renderer->DrawFrame();
}

void TouchEvent(const size_t event, const float x, const float y)
{
	LOGI("TOUCH!");
}

//========================================================================================================================

void StdExceptionCatched(std::exception& e, JNIEnv* env)
{
	LOGE("Exception has been catched: %s", e.what());
	ErrorHandler::Terminate(env);
}

void UnknownExceptionCatched(JNIEnv* env)
{
	LOGE("Unknown exception has been catched");
	ErrorHandler::Terminate(env);
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
		Init(env);
	}
	catch (std::exception& e)
	{
		StdExceptionCatched(e, env);
	}
	catch(...)
	{
		UnknownExceptionCatched(env);
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
		StdExceptionCatched(e, env);
	}
	catch(...)
	{
		UnknownExceptionCatched(env);
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
		StdExceptionCatched(e, env);
	}
	catch(...)
	{
		UnknownExceptionCatched(env);
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
		StdExceptionCatched(e, env);
	}
	catch(...)
	{
		UnknownExceptionCatched(env);
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
		StdExceptionCatched(e, env);
	}
	catch(...)
	{
		UnknownExceptionCatched(env);
	}

}

} // Pacman namespace
