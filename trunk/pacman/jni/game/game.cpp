#include "game.h"
#include "main.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "input_manager.h"
#include "map.h"
#include "shader_program.h"
#include "scene_node.h"
#include "texture.h"
#include "spritesheet.h"
#include "frame_animator.h"

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
    SceneManager& sceneManager = engine->GetSceneManager();
    InputManager& inputManager = engine->GetInputManager();

    inputManager.SetListener(gGame);

    /*
    shaderProgram = std::make_shared<ShaderProgram>(kVertexShader, kFragmentShader);
    shaderProgram->Link();

    testSprite = new Sprite(100.0f, 100.0f, Color::kRed, Color::kGreen, Color::kBlue, Color::kRed, shaderProgram);

    node = std::make_shared<SceneNode>(*testSprite, Math::Vector2f(100.0f, 50.0f));
    sceneManager->AttachNode(node);
*/

    Map map(assetManager.LoadTextFile("map.json"));
    map.AttachToScene(sceneManager);

    std::string spritesheetData = assetManager.LoadTextFile("spritesheet1.json");
    SpriteSheet spriteSheet(spritesheetData);
    size_t actorsSize = map.GetCellSize() + (map.GetCellSize() / 2);

    std::shared_ptr<Sprite> sprite_pacman_0 = spriteSheet.MakeSprite("pacman_anim_0", SpriteRegion(0, 0, actorsSize, actorsSize));
    std::shared_ptr<Sprite> sprite_pacman_1 = spriteSheet.MakeSprite("pacman_anim_1", SpriteRegion(0, 0, actorsSize, actorsSize));
    std::shared_ptr<Sprite> sprite_pacman_2 = spriteSheet.MakeSprite("pacman_anim_2", SpriteRegion(0, 0, actorsSize, actorsSize));
    
    std::vector<std::shared_ptr<Sprite>> frames;
    frames.reserve(5);
    frames.push_back(sprite_pacman_0);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_2);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_0);
    
    mPacmanAnimator = std::make_shared<FrameAnimator>(frames, 100);
    auto pos = map.GetCellPosition(0, 0);
    mPacmanNode = std::make_shared<SceneNode>(mPacmanAnimator, Math::Vector2f((float)pos.GetX(), (float)pos.GetY()));
    sceneManager.AttachNode(mPacmanNode);

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
    Engine* engine = GetEngine();
    InputManager& inputManager = engine->GetInputManager();

    inputManager.SetListener(nullptr);
}

void Game::OnUpdate(const uint64_t dt)
{
    mPacmanAnimator->Update(dt);

    if (mPacmanNode->GetPosition().GetX() > 400.0f)
        mPacmanNode->Translate(Math::Vector2f(68.0f, 144.0f));
    mPacmanNode->Move(Math::Vector2f(5.0f, 0.0f));
    Math::Vector2f pos = mPacmanNode->GetPosition();
    //LOGI("x: %f, y: %f", pos.GetX(), pos.GetY());
}

void Game::OnGesture(const GestureType gestureType)
{
    switch (gestureType)
    {
    case GestureType::LeftSwipe:
        LOGI("Left swipe");
        break;
    case GestureType::RightSwipe:
        LOGI("Right swipe");
        break;
    case GestureType::TopSwipe:
        LOGI("Top swipe");
        break;
    case GestureType::BottomSwipe:
        LOGI("Bottom swipe");
        break;
    case GestureType::None:
        LOGI("Empty swipe");
        break;
    default:
        LOGI("Unknown swipe");
        break;
    }
}

} // Pacman namespace