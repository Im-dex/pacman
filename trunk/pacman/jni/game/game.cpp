#include "game.h"

#include "main.h"
#include "engine.h"
#include "common.h"
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
#include "ghost.h"
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

static void showGameOverInfo(const bool loose)
{
    GetEngine().ShowInfo("If you interested, contact me, please:\r\n"
                         "m@il: tsukanov.anton@gmail.com\r\n"
                         "skype: im_dex", loose ? "You loooooooooose" : "You won!!!", true);
}

static bool PacmanGhostCollision(const GhostId ghostId)
{
    Game& game = GetGame();
    Ghost& ghost = game.GetAIController().GetGhost(ghostId);

    static const uint64_t kResumeInterval = 1000;
    static const auto resumeEvent = []() -> ActionResult
    {
        GetGame().Resume();
        return ActionResult::Unregister;
    };

    if (ghost.GetState() == GhostState::Frightened)
    {
        game.Pause();
        game.GetAIController().OnGhostDead(ghostId);
        game.GetScheduler().RegisterEvent(resumeEvent, kResumeInterval, false);
        return true;
    }
    else
    {
        PacmanController& pacmanController = game.GetPacmanController();
        AIController& aiController = game.GetAIController();

        game.Pause();
        const bool canContinue = pacmanController.OnPacmanFail();
        if (!canContinue)
        {
            showGameOverInfo(true);
        }
        else
        {
            pacmanController.ResetState();
            aiController.ResetState();
            game.GetScheduler().RegisterEvent(resumeEvent, kResumeInterval, false);
        }

        return false;
    }
}

void Game::OnStart(Engine& engine)
{
    mPause = false;
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
        const GhostId ghostId = MakeEnum<GhostId>(static_cast<EnumType<GhostId>::value>(i));
        mAIController->GetGhost(ghostId).GetActor()->AttachToScene(sceneManager);
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
    if (!mPause)
    {
        mPacmanController->Update(dt);
        mAIController->Update(dt);
        mScheduler->UpdateTriggers();
    }
    mScheduler->UpdateEvents(dt);
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

void Game::ShowMessage(const std::string& message) const
{
    GetEngine().ShowMessage(message);
}

void Game::InitActionsAndTriggers()
{
    const CellIndex leftTunnelExit = mMap->GetLeftTunnelExit();
    const CellIndex rightTunnelExit = mMap->GetRightTunnelExit();
 
    const std::shared_ptr<Actor> pacman = mPacmanController->GetActor();

    // create dots eating action
    const auto pacmanEatAction = []() -> ActionResult
    {
        DotsGrid& dotsGrid = GetGame().GetDotsGrid();
        const CellIndexArray pacmanCells = GetGame().GetSharedDataManager().GetPacmanCells();
        if (pacmanCells.size() == 1) // eat if pacman stays on one cell only
           dotsGrid.HideDot(pacmanCells[0]);
        return ActionResult::None;
    };
 
    // middle tunnels link actions
    // left
    const auto leftTunnelAction = [leftTunnelExit, rightTunnelExit]() -> ActionResult
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
    const auto rightTunnelAction = [leftTunnelExit, rightTunnelExit]() -> ActionResult
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

    // collision pacman with ghost
    const auto pacmanGhostCollisionAction = []() -> ActionResult
    {
        SharedDataManager& sharedDataManager = GetGame().GetSharedDataManager();
        const CellIndexArray pacmanCells = sharedDataManager.GetPacmanCells();

        for (EnumType<GhostId>::value i = 0; i < kGhostsCount; i++)
        {
            const GhostId ghostId = MakeEnum<GhostId>(i);
            const CellIndexArray ghostCells = sharedDataManager.GetGhostCells(ghostId);
            if ((ghostCells.size() == pacmanCells.size()) && 
                (!IsDifferent(ghostCells, pacmanCells)))
            {
                const bool continueCheck = PacmanGhostCollision(ghostId);
                if (!continueCheck)
                    return ActionResult::None;
            }
        }
        return ActionResult::None;
    };

    // check how much dots eaten
    const auto eatenDotsCountAction = []() -> ActionResult
    {
        Game& game = GetGame();
        DotsGrid& dotsGrid = game.GetDotsGrid();
        if (dotsGrid.GetEatenDotsCount() == dotsGrid.GetDotsCount())
        {
            game.Pause();
            showGameOverInfo(false);
        }

        return ActionResult::None;
    };
 
    // register actions and triggers
    mScheduler->RegisterTrigger(pacmanEatAction);
    mScheduler->RegisterTrigger(leftTunnelAction);
    mScheduler->RegisterTrigger(rightTunnelAction);
    mScheduler->RegisterTrigger(pacmanGhostCollisionAction);
    mScheduler->RegisterTrigger(eatenDotsCountAction);
}

} // Pacman namespace