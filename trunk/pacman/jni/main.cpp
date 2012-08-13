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
#include "engine.h"

#include <memory>
#include <numeric>
#include <unistd.h>

namespace Pacman {

Renderer* renderer = nullptr;
std::shared_ptr<SceneManager> sceneManager = nullptr;
Sprite* testSprite = nullptr;
std::shared_ptr<Sprite> testSprite2 = nullptr;
std::shared_ptr<ShaderProgram> shaderProgram;
std::shared_ptr<ShaderProgram> shaderProgram2;
std::shared_ptr<SceneNode> node;
std::shared_ptr<SceneNode> node2;
//UpdateThread uthread;
Timer timer;

static std::shared_ptr<Engine> gEngine = nullptr;

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

#define posisi "vPosition"

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

void Init(const ScreenSize screenSize, const ScreenDensity screenDensity)
{
	gEngine = std::make_shared<Engine>(screenSize, screenDensity);
	renderer = new Renderer();
}

void DeInit()
{
	gEngine->Deinit();
	shaderProgram = nullptr;
	shaderProgram2 = nullptr;
	delete testSprite;
	testSprite2 = nullptr;
	sceneManager = nullptr;
	delete renderer;
}

static uint64_t startTime = 0;

void ResizeViewport(const size_t width, const size_t heigth)
{
	gEngine->Init(width, heigth);

	LOGI("w: %d, h: %d", width, heigth);
	renderer->Init(width, heigth);
	sceneManager = std::make_shared<SceneManager>(static_cast<const float>(width), static_cast<const float>(heigth));
	renderer->SetSceneManager(sceneManager);

	AssetManager& assetManager = gEngine->GetAssetManager();
/*
	shaderProgram = std::make_shared<ShaderProgram>(kVertexShader, kFragmentShader);
	shaderProgram->Link();

	testSprite = new Sprite(100.0f, 100.0f, Color::kRed, Color::kGreen, Color::kBlue, Color::kRed, shaderProgram);

	node = std::make_shared<SceneNode>(*testSprite, Math::Vector2f(100.0f, 50.0f));
	sceneManager->AttachNode(node);
*/

	Map map;
	map.Load(assetManager.LoadTextFileFromRoot("map.json"), width, heigth);
	map.AttachToScene(sceneManager);

	// texture
	auto texture = assetManager.LoadTexture("cherry.png", TextureFiltering::Bilinear, TextureRepeat::None);

	shaderProgram2 = std::make_shared<ShaderProgram>(kVertexShader2, kFragmentShader2);
	shaderProgram2->Link();

	SpriteRegion region2(SpriteRegion::Position::kZero, 24, 24);
	testSprite2 = std::make_shared<Sprite>(region2, texture, shaderProgram2, true);

	node2 = std::make_shared<SceneNode>(testSprite2, Math::Vector2f(32.0f, 16.0f));
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

void DrawFrame()
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

void TouchEvent(const size_t event, const float x, const float y)
{
	LOGI("TOUCH!");
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
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_init(JNIEnv * env, jobject obj, jint screenSize, jint screenDensity);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_deinit(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_resizeViewport(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_drawFrame(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_touchEvent(JNIEnv * env, jobject obj, jint event, jfloat x, jfloat y);
}

JNIEXPORT void JNICALL Java_com_imdex_pacman_NativeLib_init(JNIEnv* env, jobject obj, jint screenSize, jint screenDensity)
{
	try
	{
		LOGI("size: %d, dens: %d", screenSize, screenDensity);
		PACMAN_CHECK_ERROR((screenSize > 0) && (screenSize < kScreenSizesCount), ErrorCode::BadArgument);
		PACMAN_CHECK_ERROR((screenDensity > 0) && (screenDensity < kScreenDensitiesCount), ErrorCode::BadArgument);
		const uint8_t screenSize2 = static_cast<uint8_t>(screenSize);
		const uint8_t screenDensity2 = static_cast<uint8_t>(screenDensity);
		Init(static_cast<ScreenSize>(screenSize2), static_cast<ScreenDensity>(screenDensity2));
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
