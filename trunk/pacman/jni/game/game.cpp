#include "game.h"

#include "log.h"
#include "main.h"
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

static CellIndex GetPacmanCellIndex(SchedulerContext& context, const std::shared_ptr<Map> map,
                                    const std::shared_ptr<Actor> pacman)
{
    static const std::string kPacmanCellIndexName = "pacman_cell";
    const boost::any indexValue = context.GetValue(kPacmanCellIndexName);
    if (!indexValue.empty())
        return boost::any_cast<CellIndex>(indexValue);

    const CellIndex pacmanCellIndex = map->FindCell(pacman->GetRegion());
    context.SetValue(kPacmanCellIndexName, pacmanCellIndex);
    return pacmanCellIndex;
}

void Game::OnLoad()
{
    GameLoader loader;
    mScheduler = std::unique_ptr<Scheduler>(new Scheduler());

    LOGI("LOAD!");
    Engine* engine = GetEngine();
    AssetManager& assetManager = engine->GetAssetManager();
    SceneManager& sceneManager = engine->GetSceneManager();
    InputManager& inputManager = engine->GetInputManager();
    
    const uint16_t cellSize = CalcCellSize(assetManager);
    inputManager.SetListener(gGame);

    std::shared_ptr<Map> map = loader.LoadMap("map.json", cellSize);
    map->AttachToScene(sceneManager);

    std::shared_ptr<SpriteSheet> spriteSheet = assetManager.LoadSpriteSheet("spritesheet1.json");

    std::shared_ptr<DotsGrid> dots = loader.MakeDotsGrid(map, spriteSheet);
    dots->AttachToScene(sceneManager);

    const uint16_t actorSize = CalcActorSize(cellSize);
    mPacmanController = std::unique_ptr<PacmanController>(new PacmanController(loader, actorSize, map, spriteSheet));
    mPacmanController->GetActor()->AttachToScene(sceneManager);

    InitActionsAndTriggers(map, dots);
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
    mScheduler->Update(dt);
    mPacmanController->Update(dt);
    //mBlinky->Update(dt);
}

void Game::OnGesture(const GestureType gestureType)
{
    switch (gestureType)
    {
    case GestureType::LeftSwipe:
        mPacmanController->ChangeDirection(MoveDirection::Left);
        break;
    case GestureType::RightSwipe:
        mPacmanController->ChangeDirection(MoveDirection::Right);
        break;
    case GestureType::TopSwipe:
        mPacmanController->ChangeDirection(MoveDirection::Up);
        break;
    case GestureType::BottomSwipe:
        mPacmanController->ChangeDirection(MoveDirection::Down);
        break;
    case GestureType::None:
    default:
        break;
    }
}

void Game::InitActionsAndTriggers(const std::shared_ptr<Map> map, const std::shared_ptr<DotsGrid> dots)
{
    const CellIndex leftTunnelExit = map->GetLeftTunnelExit();
    const CellIndex rightTunnelExit = map->GetRightTunnelExit();
 
    const std::shared_ptr<Actor> pacman = mPacmanController->GetActor();

     // create dots eating action
     auto eatAction = [pacman, map, dots](SchedulerContext& context) -> ActionResult
     {
         const CellIndex pacmanCellIndex = GetPacmanCellIndex(context, map, pacman);
         dots->HideDot(pacmanCellIndex);
         return ActionResult::None;
     };
 
     std::shared_ptr<Action> pacmanEatAction = std::make_shared<Action>(eatAction);
 
     // middle tunnels link triggers
     // left
     auto leftCondition = [pacman, map, leftTunnelExit](SchedulerContext& context) -> bool
     {
         const CellIndex pacmanCellIndex = GetPacmanCellIndex(context, map, pacman);
         return (pacmanCellIndex == leftTunnelExit) && (pacman->GetDirection() == MoveDirection::Left);
     };
 
     auto leftAction = [pacman, rightTunnelExit](SchedulerContext& context) -> ActionResult
     {
         pacman->Translate(rightTunnelExit);
         pacman->Move(pacman->GetDirection(), Actor::kMax);
         return ActionResult::None;
     };
 
     // right
     auto rightCondition = [pacman, map, rightTunnelExit](SchedulerContext& context) -> bool
     {
         const CellIndex pacmanCellIndex = GetPacmanCellIndex(context, map, pacman);
         return (pacmanCellIndex == rightTunnelExit)  && (pacman->GetDirection() == MoveDirection::Right);
     };
 
     auto rightAction = [pacman, leftTunnelExit](SchedulerContext& context) -> ActionResult
     {
         pacman->Translate(leftTunnelExit);
         pacman->Move(pacman->GetDirection(), Actor::kMax);
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