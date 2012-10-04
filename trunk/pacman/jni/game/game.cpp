#include "game.h"

#include "main.h"
#include "log.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "input_manager.h"
#include "loader.h"
#include "actor.h"
#include "scene_node.h"
#include "spritesheet.h"
#include "boost/any.hpp"

static std::shared_ptr<Pacman::Game> gGame = nullptr;

void PacmanSetEngineListener(Pacman::Engine& engine)
{
    gGame = std::make_shared<Pacman::Game>();
    engine.SetListener(gGame);
}

namespace Pacman {

static FORCEINLINE Size CalcCellSize(const AssetManager& assetManager)
{
    static const Size kBaseCellSize = 8;
    return kBaseCellSize * assetManager.GetMultiplier();
}

static FORCEINLINE Size CalcActorSize(const Size cellSize)
{
    return cellSize + (cellSize / 2);
}

static CellIndexArray GetPacmanCellsIndices(SchedulerContext& context, const std::shared_ptr<Map>& map,
                                            const std::shared_ptr<Actor>& pacman)
{
    static const std::string kPacmanCellsIndicesName = "pacman_cells_indices";
    const boost::any cellsIndicesValue = context.GetValue(kPacmanCellsIndicesName);
    if (!cellsIndicesValue.empty())
        return boost::any_cast<CellIndexArray>(cellsIndicesValue);

    const CellIndexArray pacmanCellsIndices = map->FindCells(pacman->GetRegion());
    context.SetValue(kPacmanCellsIndicesName, pacmanCellsIndices);
    return pacmanCellsIndices;
}

void Game::OnStart(Engine& engine)
{
    GameLoader loader;
    mScheduler = std::unique_ptr<Scheduler>(new Scheduler());

    LogI("LOAD!");
    AssetManager& assetManager = engine.GetAssetManager();
    SceneManager& sceneManager = engine.GetSceneManager();
    InputManager& inputManager = engine.GetInputManager();
    
    const Size cellSize = CalcCellSize(assetManager);
    const Size actorSize = CalcActorSize(cellSize);
    inputManager.SetListener(gGame);

    std::shared_ptr<Map> map = loader.LoadMap("map.json", cellSize);
    map->AttachToScene(sceneManager);

    std::shared_ptr<SpriteSheet> spriteSheet = assetManager.LoadSpriteSheet("spritesheet1.json");

    std::shared_ptr<DotsGrid> dots = loader.MakeDotsGrid(map, spriteSheet);
    dots->AttachToScene(sceneManager);

    mPacmanController = std::make_shared<PacmanController>(loader, actorSize, map, spriteSheet);
    mAIController = std::make_shared<AIController>(loader, actorSize, map, spriteSheet);

    mPacmanController->GetActor()->AttachToScene(sceneManager);
    for (size_t i = 0; i < AIController::kGhostsCount; i++)
    {
        mAIController->GetActor(i)->AttachToScene(sceneManager);
    }

    InitActionsAndTriggers(map, dots);
}

void Game::OnStop(Engine& engine)
{
    LogI("UNLOAD!");
    engine.SetListener(nullptr);
    gGame = nullptr;
}

void Game::OnUpdate(Engine& engine, const uint64_t dt)
{
    mScheduler->Update(dt);
    mPacmanController->Update(dt);
    mAIController->Update(dt);
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

void Game::InitActionsAndTriggers(const std::shared_ptr<Map>& map, const std::shared_ptr<DotsGrid>& dots)
{
    const CellIndex leftTunnelExit = map->GetLeftTunnelExit();
    const CellIndex rightTunnelExit = map->GetRightTunnelExit();
 
    const std::shared_ptr<Actor> pacman = mPacmanController->GetActor();

     // create dots eating action
     auto eatAction = [pacman, map, dots](SchedulerContext& context) -> ActionResult
     {
         const CellIndexArray pacmanCellsIndices = GetPacmanCellsIndices(context, map, pacman);
         if (pacmanCellsIndices.size() == 1) // eat if pacman stays on one cell only
            dots->HideDot(pacmanCellsIndices[0]);
         return ActionResult::None;
     };
 
     std::shared_ptr<Action> pacmanEatAction = std::make_shared<Action>(eatAction);
 
     // middle tunnels link triggers
     // left
     auto leftCondition = [pacman, map, leftTunnelExit](SchedulerContext& context) -> bool
     {
         const CellIndexArray pacmanCellsIndices = GetPacmanCellsIndices(context, map, pacman);
         if (pacmanCellsIndices.size() == 1) // move if pacman stays on the one cell only
            return (pacmanCellsIndices[0] == leftTunnelExit) && (pacman->GetDirection() == MoveDirection::Left);
         return false;
     };
 
     auto leftAction = [pacman, rightTunnelExit, mPacmanController](SchedulerContext& context) -> ActionResult
     {
         pacman->TranslateTo(rightTunnelExit);
         //pacman->Move(pacman->GetDirection(), Actor::kMax);
         mPacmanController->ChangeDirection(pacman->GetDirection());
         return ActionResult::None;
     };
 
     // right
     auto rightCondition = [pacman, map, rightTunnelExit](SchedulerContext& context) -> bool
     {
         const CellIndexArray pacmanCellsIndices = GetPacmanCellsIndices(context, map, pacman);
         if (pacmanCellsIndices.size() == 1) // move if pacman stays on the one cell only
            return (pacmanCellsIndices[0] == rightTunnelExit)  && (pacman->GetDirection() == MoveDirection::Right);
         return false;
     };
 
     auto rightAction = [pacman, leftTunnelExit, mPacmanController](SchedulerContext& context) -> ActionResult
     {
         pacman->TranslateTo(leftTunnelExit);
         //pacman->Move(pacman->GetDirection(), Actor::kMax);
         mPacmanController->ChangeDirection(pacman->GetDirection());
         return ActionResult::None;
     };
 
     std::shared_ptr<Trigger> leftTunnelTrigger = std::make_shared<Trigger>(leftCondition, leftAction);
     std::shared_ptr<Trigger> rightTunnelTrigger = std::make_shared<Trigger>(rightCondition, rightAction);
 
     // register actions and triggers
     mScheduler->RegisterAction(pacmanEatAction, 0, true);
     mScheduler->RegisterTrigger(leftTunnelTrigger);
     mScheduler->RegisterTrigger(rightTunnelTrigger);
}

} // Pacman namespace