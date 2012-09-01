#include "game.h"

#include <memory>

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
#include "actors_manager.h"

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

static size_t startTmp = 0;
static size_t startTmp2 = 0;

namespace Pacman {

void Game::OnLoad()
{
    Engine* engine = GetEngine();
    AssetManager& assetManager = engine->GetAssetManager();
    SceneManager& sceneManager = engine->GetSceneManager();
    InputManager& inputManager = engine->GetInputManager();

    inputManager.SetListener(gGame);

    std::vector<DotType> dotsInfo;
    std::shared_ptr<Map> map = std::make_shared<Map>(assetManager.LoadTextFile("map.json"), dotsInfo);
    map->AttachToScene(sceneManager);

    std::string spritesheetData = assetManager.LoadTextFile("spritesheet1.json");
    SpriteSheet spriteSheet(spritesheetData);
    size_t actorsSize = map->GetCellSize() + (map->GetCellSize() / 2);

    size_t dotSize = map->GetCellSize() / 2;

    std::shared_ptr<Sprite> sprite_pacman_0 = spriteSheet.MakeSprite("pacman_anim_0", SpriteRegion(0, 0, actorsSize, actorsSize));
    std::shared_ptr<Sprite> sprite_pacman_1 = spriteSheet.MakeSprite("pacman_anim_1", SpriteRegion(0, 0, actorsSize, actorsSize));
    std::shared_ptr<Sprite> sprite_pacman_2 = spriteSheet.MakeSprite("pacman_anim_2", SpriteRegion(0, 0, actorsSize, actorsSize));
    
    std::shared_ptr<Sprite> dot = spriteSheet.MakeSprite("dot", SpriteRegion(0, 0, dotSize, dotSize));
    auto node = std::make_shared<SceneNode>(dot, Math::Vector2f(100.f, 200.f));
    sceneManager.AttachNode(node);

    std::vector<std::shared_ptr<Sprite>> frames;
    frames.reserve(5);
    frames.push_back(sprite_pacman_0);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_2);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_0);
    
    mPacmanAnimator = std::make_shared<FrameAnimator>(frames, 100);
    auto pos = map->GetCellCenterPos(1, 1);
    startTmp = pos.GetX() - actorsSize/2;
    startTmp2 = pos.GetY() - actorsSize/2;
    mPacmanNode = std::make_shared<SceneNode>(mPacmanAnimator, Math::Vector2f((float)pos.GetX() - actorsSize/2, (float)pos.GetY() - actorsSize/2));
    sceneManager.AttachNode(mPacmanNode);
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
        mPacmanNode->Translate(Math::Vector2f((float)startTmp, (float)startTmp2));
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