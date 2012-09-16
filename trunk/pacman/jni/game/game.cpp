#include "game.h"

#include "log.h"
#include "main.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"
#include "input_manager.h"
#include "shader_program.h"
#include "scene_node.h"
#include "texture.h"
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
                                    const std::shared_ptr<PacmanActor> pacman)
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
    LOGI("LOAD!");
    Engine* engine = GetEngine();
    AssetManager& assetManager = engine->GetAssetManager();
    SceneManager& sceneManager = engine->GetSceneManager();
    InputManager& inputManager = engine->GetInputManager();
    
    const uint16_t cellSize = CalcCellSize(assetManager);
    inputManager.SetListener(gGame);

    std::shared_ptr<Map> map = mLoader.LoadMap("map.json", cellSize);
    map->AttachToScene(sceneManager);

    std::shared_ptr<SpriteSheet> spriteSheet = assetManager.LoadSpriteSheet("spritesheet1.json");

    std::shared_ptr<DotsGrid> dots = mLoader.MakeDotsGrid(map, spriteSheet);
    dots->AttachToScene(sceneManager);

    const uint16_t actorSize = CalcActorSize(cellSize);
    mPacman = mLoader.LoadPacmanActor("pacman.json", actorSize, spriteSheet, map);
    mPacman->AttachToScene(sceneManager);

    mBlinky = mLoader.LoadGhostActor("blinky.json", actorSize, spriteSheet, map);
    mBlinky->AttachToScene(sceneManager);

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
    mScheduler.Update(dt);
    mPacman->Update(dt);
    //mBlinky->Update(dt);
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

void Game::InitActionsAndTriggers(const std::shared_ptr<Map> map, const std::shared_ptr<DotsGrid> dots)
{
    const CellIndex leftTunnelExit = map->GetLeftTunnelExit();
    const CellIndex rightTunnelExit = map->GetRightTunnelExit();

    // create dots eating action
    auto eatAction = [mPacman, map, dots](SchedulerContext& context) -> ActionResult
    {
        const CellIndex pacmanCellIndex = GetPacmanCellIndex(context, map, mPacman);
        dots->HideDot(pacmanCellIndex);
        return ActionResult::None;
    };

    std::shared_ptr<Action> pacmanEatAction = std::make_shared<Action>(eatAction);

    // middle tunnels link triggers
    // left
    auto leftCondition = [mPacman, map, leftTunnelExit](SchedulerContext& context) -> bool
    {
        const CellIndex pacmanCellIndex = GetPacmanCellIndex(context, map, mPacman);
        return (pacmanCellIndex == leftTunnelExit) && (mPacman->GetDirection() == PacmanMoveDirection::Left);
    };

    auto leftAction = [mPacman, rightTunnelExit](SchedulerContext& context) -> ActionResult
    {
        mPacman->TranslateTo(rightTunnelExit);
        return ActionResult::None;
    };

    // right
    auto rightCondition = [mPacman, map, rightTunnelExit](SchedulerContext& context) -> bool
    {
        const CellIndex pacmanCellIndex = GetPacmanCellIndex(context, map, mPacman);
        return (pacmanCellIndex == rightTunnelExit)  && (mPacman->GetDirection() == PacmanMoveDirection::Right);
    };

    auto rightAction = [mPacman, leftTunnelExit](SchedulerContext& context) -> ActionResult
    {
        mPacman->TranslateTo(leftTunnelExit);
        return ActionResult::None;
    };

    std::shared_ptr<Trigger> leftTunnelTrigger = std::make_shared<Trigger>(leftCondition, leftAction);
    std::shared_ptr<Trigger> rightTunnelTrigger = std::make_shared<Trigger>(rightCondition, rightAction);

    // register actions and triggers
    mScheduler.RegisterAction(pacmanEatAction, 0, true);
    mScheduler.RegisterTrigger(leftTunnelTrigger);
    mScheduler.RegisterTrigger(rightTunnelTrigger);
}

} // Pacman namespace