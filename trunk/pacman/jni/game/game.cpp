#include "game.h"

#include "main.h"
#include "log.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "input_manager.h"
#include "pacman_controller.h"
#include "ai_controller.h"
#include "shared_data_manager.h"
#include "map.h"
#include "dots_grid.h"
#include "scheduler.h"
#include "loader.h"
#include "actor.h"
#include "scene_node.h"
#include "spritesheet.h"

static std::shared_ptr<Pacman::Game> gGame = nullptr;

void PacmanSetEngineListener(Pacman::Engine& engine)
{
    gGame = std::make_shared<Pacman::Game>();
    engine.SetListener(gGame);
}

namespace Pacman {

Game& GetGame()
{
    return *gGame;
}

static FORCEINLINE Size CalcCellSize(const AssetManager& assetManager)
{
    static const Size kBaseCellSize = 8;
    return kBaseCellSize * assetManager.GetMultiplier();
}

static FORCEINLINE Size CalcActorSize(const Size cellSize)
{
    return cellSize + (cellSize / 2);
}

void Game::OnStart(Engine& engine)
{
    mLoader = std::unique_ptr<GameLoader>(new GameLoader());
    mScheduler = std::unique_ptr<Scheduler>(new Scheduler());
    mSharedDataManager = std::unique_ptr<SharedDataManager>(new SharedDataManager());

    AssetManager& assetManager = engine.GetAssetManager();
    SceneManager& sceneManager = engine.GetSceneManager();
    InputManager& inputManager = engine.GetInputManager();
    
    const Size cellSize = CalcCellSize(assetManager);
    const Size actorSize = CalcActorSize(cellSize);
    inputManager.SetListener(gGame);

    mMap = mLoader->LoadMap("map.json", cellSize);
    mMap->AttachToScene(sceneManager);

    std::shared_ptr<SpriteSheet> spriteSheet = assetManager.LoadSpriteSheet("spritesheet1.json");

    mDotsGrid = mLoader->MakeDotsGrid(spriteSheet);
    mDotsGrid->AttachToScene(sceneManager);

    mPacmanController = std::unique_ptr<PacmanController>(new PacmanController(actorSize, spriteSheet));
    mAIController = std::unique_ptr<AIController>(new AIController(actorSize, spriteSheet));

    mPacmanController->GetActor()->AttachToScene(sceneManager);
    for (size_t i = 0; i < kGhostsCount; i++)
    {
        mAIController->GetActor(i)->AttachToScene(sceneManager);
    }

    InitActionsAndTriggers();
}

void Game::OnStop(Engine& engine)
{
    engine.SetListener(nullptr);
    gGame = nullptr;
}

void Game::OnUpdate(Engine& engine, const uint64_t dt)
{
    mPacmanController->Update(dt);
    mAIController->Update(dt);
    mScheduler->Update(dt);
    mSharedDataManager->Reset();
}

void Game::OnGesture(const GestureType gestureType)
{
    MoveDirection newDirection = MoveDirection::None;
    switch (gestureType)
    {
    case GestureType::LeftSwipe:
        newDirection = MoveDirection::Left;
        break;
    case GestureType::RightSwipe:
        newDirection = MoveDirection::Right;
        break;
    case GestureType::TopSwipe:
        newDirection = MoveDirection::Up;
        break;
    case GestureType::BottomSwipe:
        newDirection = MoveDirection::Down;
        break;
    case GestureType::None:
    default:
        break;
    }

    if ((newDirection != MoveDirection::None) && 
        (mPacmanController->GetActor()->GetDirection() != newDirection))
    {
        mPacmanController->ChangeDirection(newDirection);
    }
}

void Game::InitActionsAndTriggers()
{
    const CellIndex leftTunnelExit = mMap->GetLeftTunnelExit();
    const CellIndex rightTunnelExit = mMap->GetRightTunnelExit();
 
    const std::shared_ptr<Actor> pacman = mPacmanController->GetActor();

    // create dots eating action
    auto eatAction = []() -> ActionResult
    {
        DotsGrid& dotsGrid = GetGame().GetDotsGrid();
        const CellIndexArray pacmanCells = GetGame().GetSharedDataManager().GetPacmanCells();
        if (pacmanCells.size() == 1) // eat if pacman stays on one cell only
           dotsGrid.HideDot(pacmanCells[0]);
        return ActionResult::None;
    };
 
    // middle tunnels link actions
    // left
    auto leftAction = [leftTunnelExit, rightTunnelExit]() -> ActionResult
    {
        const CellIndexArray pacmanCells = GetGame().GetSharedDataManager().GetPacmanCells();
        if (pacmanCells.size() == 1) // move if pacman stays on the one cell only
        {
            const std::shared_ptr<Actor> pacman = GetGame().GetPacmanController().GetActor();
            if ((pacmanCells[0] == leftTunnelExit) && (pacman->GetDirection() == MoveDirection::Left))
                GetGame().GetPacmanController().TranslateTo(rightTunnelExit);
        }
        return ActionResult::None;
    };
 
    // right
    auto rightAction = [leftTunnelExit, rightTunnelExit]() -> ActionResult
    {
        const CellIndexArray pacmanCells = GetGame().GetSharedDataManager().GetPacmanCells();
        if (pacmanCells.size() == 1) // move if pacman stays on the one cell only
        {
            const std::shared_ptr<Actor> pacman = GetGame().GetPacmanController().GetActor();
            if ((pacmanCells[0] == rightTunnelExit)  && (pacman->GetDirection() == MoveDirection::Right))
                GetGame().GetPacmanController().TranslateTo(leftTunnelExit);
        }
        return ActionResult::None;
    };
 
    std::shared_ptr<Action> pacmanEatAction = std::make_shared<Action>(eatAction);
    std::shared_ptr<Action> leftTunnelAction = std::make_shared<Action>(leftAction);
    std::shared_ptr<Action> rightTunnelAction = std::make_shared<Action>(rightAction);
 
    // register actions and triggers
    mScheduler->RegisterAction(pacmanEatAction, 0, true);
    mScheduler->RegisterAction(leftTunnelAction, 0, true);
    mScheduler->RegisterAction(rightTunnelAction, 0, true);
}

} // Pacman namespace