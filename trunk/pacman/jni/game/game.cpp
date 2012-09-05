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
#include "actor.h"
#include "dots_grid.h"

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
    mScheduler = std::unique_ptr<Scheduler>(new Scheduler());

    std::vector<DotType> dotsInfo;
    std::shared_ptr<Map> map = std::make_shared<Map>(assetManager.LoadTextFile("map.json"), dotsInfo);
    map->AttachToScene(sceneManager);

    mActorsManager = std::unique_ptr<ActorsManager>(new ActorsManager(map));

    std::string spritesheetData = assetManager.LoadTextFile("spritesheet1.json");
    SpriteSheet spriteSheet(spritesheetData);

    std::shared_ptr<DotsGrid> dots = std::make_shared<DotsGrid>(dotsInfo, map, spriteSheet);
    dots->AttachToScene(sceneManager);

    size_t actorsSize = map->GetCellSize() + (map->GetCellSize() / 2);
    std::shared_ptr<Sprite> sprite_pacman_0 = spriteSheet.MakeSprite("pacman_anim_0", SpriteRegion(0, 0, actorsSize, actorsSize));
    std::shared_ptr<Sprite> sprite_pacman_1 = spriteSheet.MakeSprite("pacman_anim_1", SpriteRegion(0, 0, actorsSize, actorsSize));
    std::shared_ptr<Sprite> sprite_pacman_2 = spriteSheet.MakeSprite("pacman_anim_2", SpriteRegion(0, 0, actorsSize, actorsSize));

    std::vector<std::shared_ptr<Sprite>> frames;
    frames.reserve(4);
    frames.push_back(sprite_pacman_0);
    frames.push_back(sprite_pacman_1);
    frames.push_back(sprite_pacman_2);
    frames.push_back(sprite_pacman_1);
    //frames.push_back(sprite_pacman_0);
    
    mPacmanAnimator = std::make_shared<FrameAnimator>(frames, 55);
    std::shared_ptr<Actor> pacmanActor = std::make_shared<Actor>(assetManager.LoadTextFile("pacman.json"), mPacmanAnimator, actorsSize, map);
    mActorsManager->RegisterActor(pacmanActor);
}

void Game::OnUnload()
{
    Engine* engine = GetEngine();
    InputManager& inputManager = engine->GetInputManager();

    inputManager.SetListener(nullptr);
}

void Game::OnUpdate(const uint64_t dt)
{
    mScheduler->Update(dt);
    mPacmanAnimator->Update(dt);
    mActorsManager->Update(dt);
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