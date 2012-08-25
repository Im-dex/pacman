#include "game.h"
#include "main.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "renderer.h"
#include "map.h"
#include "shader_program.h"
#include "scene_node.h"
#include "texture.h"
#include "spritesheet.h"

#include <memory>

static std::shared_ptr<Pacman::Game> gGame = nullptr;

void OnPacmanApplicationStart()
{
	gGame = std::make_shared<Pacman::Game>();
	Pacman::GetEngine()->SetListener(gGame);
}

void OnPacmanApplicationEnd()
{
	Pacman::GetEngine()->SetListener(nullptr);
	gGame = nullptr;
}

namespace Pacman {

void Game::OnLoad()
{
	Engine* engine = GetEngine();
	AssetManager& assetManager = engine->GetAssetManager();
	Renderer& renderer = engine->GetRenderer();
	SceneManager& sceneManager = engine->GetSceneManager();

	/*
	shaderProgram = std::make_shared<ShaderProgram>(kVertexShader, kFragmentShader);
	shaderProgram->Link();

	testSprite = new Sprite(100.0f, 100.0f, Color::kRed, Color::kGreen, Color::kBlue, Color::kRed, shaderProgram);

	node = std::make_shared<SceneNode>(*testSprite, Math::Vector2f(100.0f, 50.0f));
	sceneManager->AttachNode(node);
*/

	Map map;
	map.Load(assetManager.LoadTextFile("map.json"), renderer.GetViewportWidth(), renderer.GetViewportHeight());
	map.AttachToScene(sceneManager);

	SpriteSheet spriteSheet("spritesheet1.json");

	// texture
	/*auto texture = assetManager.LoadTexture("cherry.png", TextureFiltering::Bilinear, TextureRepeat::None);

	auto shaderProgram2 = std::make_shared<ShaderProgram>(ShaderProgram::kDefaultTextureVertexShader, ShaderProgram::kDefaultTextureFragmentShader);
	shaderProgram2->Link();

	SpriteRegion region2(SpriteRegion::Position::kZero, 24, 24);
	auto testSprite2 = std::make_shared<Sprite>(region2, texture, shaderProgram2, true);

	auto node2 = std::make_shared<SceneNode>(testSprite2, Math::Vector2f(32.0f, 16.0f));
	sceneManager.AttachNode(node2);*/
}

void Game::OnUnload()
{

}

void Game::OnUpdate(const uint64_t dt)
{

}

} // Pacman namespace