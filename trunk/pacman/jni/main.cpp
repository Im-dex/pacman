#include <jni.h>

#include "base.h"
#include "error.h"
#include "renderer.h"
#include "scene_manager.h"

#include "asset_manager.h"
#include "sprite.h"
#include "shader_program.h"
#include "timer.h"
#include "scene_node.h"

#include "base.h"
//#include "update_thread.h"

#include "map.h"

#include <memory>
#include <numeric>
#include <unistd.h>

namespace Pacman {

Renderer* renderer = nullptr;
std::shared_ptr<SceneManager> sceneManager = nullptr;
Sprite* testSprite = nullptr;
Sprite* testSprite2 = nullptr;
std::shared_ptr<ShaderProgram> shaderProgram;
std::shared_ptr<ShaderProgram> shaderProgram2;
std::shared_ptr<SceneNode> node;
std::shared_ptr<SceneNode> node2;
//UpdateThread uthread;
Timer timer;

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

static uint64_t startTime = 0;

void ResizeViewport(JNIEnv* env, const size_t width, const size_t heigth)
{
	LOGI("w: %d, h: %d", width, heigth);
	renderer->Init(width, heigth);
	sceneManager = std::make_shared<SceneManager>(static_cast<const float>(width), static_cast<const float>(heigth));
	renderer->SetSceneManager(sceneManager);


/*
	shaderProgram = std::make_shared<ShaderProgram>(kVertexShader, kFragmentShader);
	shaderProgram->Link();

	testSprite = new Sprite(100.0f, 100.0f, Color::kRed, Color::kGreen, Color::kBlue, Color::kRed, shaderProgram);

	node = std::make_shared<SceneNode>(*testSprite, Math::Vector2f(100.0f, 50.0f));
	sceneManager->AttachNode(node);
*/

	Map map;
	map.Load(AssetManager::LoadTextFile(env, "map.json"), width, heigth);
	map.AttachToScene(sceneManager);

	// texture
	auto texture = AssetManager::LoadTexture(env, "cherry.png", TextureFiltering::Bilinear, TextureRepeat::None);

	shaderProgram2 = std::make_shared<ShaderProgram>(kVertexShader2, kFragmentShader2);
	shaderProgram2->Link();

	testSprite2 = new Sprite(24.0f, 24.0f, texture, shaderProgram2);

	node2 = std::make_shared<SceneNode>(*testSprite2, Math::Vector2f(32.0f, 16.0f));
	sceneManager->AttachNode(node2);

	timer.Start();
	startTime = timer.GetMillisec();
}

static const float speed = 200; // pixels per second

static float kSmoothDTMillis = 17.5f;
static float movAverageDTMillis = kSmoothDTMillis;
static uint64_t lastTime = 0;

static const float movAveragePeriod = 40.0; // fps
static const float smoothFactor = 0.1f;

static uint64_t lastUpdate = 0;

void DrawFrame(JNIEnv* env)
{
/*	uint64_t cur = timer.GetMillisec();
	float dt = (cur - lastUpdate) / 1000.0f;
	lastUpdate = cur;

	Math::Vector2f pos = node2->GetPosition();
	if (pos.GetX() >= 1024.0f)
	{
		pos.SetX(0.0f);
		node2->Translate(pos);
	}*/

	//const float offset_x = speed * dt;
	//LOGI("offset by time: %f, %f", offset_x, dt);
	//node2->Move(Math::Vector2f(offset_x, 0.0f));

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
