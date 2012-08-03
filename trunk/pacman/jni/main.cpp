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
Sprite* testSprite2 = nullptr;
std::shared_ptr<ShaderProgram> shaderProgram;
std::shared_ptr<ShaderProgram> shaderProgram2;

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

static const char kVertexShader2[] = "attribute vec4 vPosition;"
									 "attribute vec2 vTexCoords;\n"
									 "uniform mat4 mModelProjectionMatrix;\n"
									 "varying vec2 vVertTexCoords;\n"
									 "void main() {\n"
									 "	gl_Position = mModelProjectionMatrix * vPosition;\n"
									 "	vVertTexCoords = vTexCoords;\n"
									 "}\n";

static const char kFragmentShader2[] = "precision mediump float;\n"
									   "uniform sampler2D colorTexture;\n"
									   "varying vec2 vVertTexCoords;\n"
							  	  	   "void main() {\n"
							  	  	   "	gl_FragColor = texture2D(colorTexture, vVertTexCoords);\n"
							  	  	   "}\n";

void Init(JNIEnv* env)
{
	renderer = new Renderer();
}

void DeInit(JNIEnv* env)
{
	shaderProgram = nullptr;
	shaderProgram2 = nullptr;
	delete testSprite;
	delete testSprite2;
	sceneManager = nullptr;
	delete renderer;
}

void ResizeViewport(JNIEnv* env, const size_t width, const size_t heigth)
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


	// texture
	auto texture = AssetManager::LoadTexture(env, "part.png", TextureFiltering::None, TextureRepeat::None);

	shaderProgram2 = std::make_shared<ShaderProgram>(kVertexShader2, kFragmentShader2);
	shaderProgram2->Link();

	testSprite2 = new Sprite(64.0f, 64.0f, texture, shaderProgram2);

	SceneNode node2(*testSprite2, Math::Vector2f(400.0f, 20.0f));
	sceneManager->AttachNode(node2);
}

void DrawFrame(JNIEnv* env)
{
	renderer->DrawFrame();
}

void TouchEvent(JNIEnv* env, const size_t event, const float x, const float y)
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
		DeInit(env);
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
		ResizeViewport(env, width, heigth);
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
		DrawFrame(env);
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
		TouchEvent(env, event, x, y);
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
