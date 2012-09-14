#include "game.h"

#include <memory>

#include "log.h"
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

static FORCEINLINE uint16_t CalcCellSize(const AssetManager& assetManager)
{
    static const uint16_t kBaseCellSize = 8;
    return kBaseCellSize * assetManager.GetMultiplier();
}

static FORCEINLINE uint16_t CalcActorSize(const uint16_t cellSize)
{
    return cellSize + (cellSize / 2);
}

void Game::OnLoad()
{
    LOGI("LOAD!");
    Engine* engine = GetEngine();
    AssetManager& assetManager = engine->GetAssetManager();
    SceneManager& sceneManager = engine->GetSceneManager();
    InputManager& inputManager = engine->GetInputManager();
    
    const uint16_t cellSize = CalcCellSize(assetManager);
    inputManager.SetListener(gGame);

    mMap = mLoader.LoadMap("map.json", cellSize);
    mMap->AttachToScene(sceneManager);

    std::shared_ptr<SpriteSheet> spriteSheet = assetManager.LoadSpriteSheet("spritesheet1.json");

    std::shared_ptr<DotsGrid> dots = mLoader.MakeDotsGrid(mMap, spriteSheet);
    dots->AttachToScene(sceneManager);

    const uint16_t actorSize = CalcActorSize(cellSize);
    mPacman = mLoader.LoadPacmanActor("pacman.json", actorSize, spriteSheet, mMap);
    mPacman->AttachToScene(sceneManager);
}

void Game::OnUnload()
{
    LOGI("UNLOAD!");
    Engine* engine = GetEngine();
    InputManager& inputManager = engine->GetInputManager();

    inputManager.SetListener(nullptr);
}

void Game::OnUpdate(const uint64_t dt)
{
    mScheduler.Update(dt);
    mPacman->Update(dt);
}

void Game::OnGesture(const GestureType gestureType)
{
    switch (gestureType)
    {
    case GestureType::LeftSwipe:
        mPacman->ChangeDirection(PacmanMoveDirection::Left);
        break;
    case GestureType::RightSwipe:
        mPacman->ChangeDirection(PacmanMoveDirection::Right);
        break;
    case GestureType::TopSwipe:
        mPacman->ChangeDirection(PacmanMoveDirection::Up);
        break;
    case GestureType::BottomSwipe:
        mPacman->ChangeDirection(PacmanMoveDirection::Down);
        break;
    case GestureType::None:
    default:
        break;
    }
}

} // Pacman namespace