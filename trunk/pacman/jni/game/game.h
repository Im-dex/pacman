#pragma once

#include <memory>

#include "engine_listeners.h"

namespace Pacman {

class GameLoader;
class Map;
class DotsGrid;
class Scheduler;
class PacmanController;
class AIController;
class SharedDataManager;

class Game : public IEngineListener, public IGestureListener
{
public:

	Game() = default;
	Game(const Game&) = delete;
	~Game() = default;

	Game& operator= (const Game&) = delete;

	virtual void OnStart(Engine& engine);

	virtual void OnStop(Engine& engine);

	virtual void OnUpdate(Engine& engine, const uint64_t dt);

    virtual void OnGesture(const GestureType gestureType);

    GameLoader& GetLoader() const
    {
        return *mLoader;
    }

    Map& GetMap() const
    {
        return *mMap;
    }

    DotsGrid& GetDotsGrid() const
    {
        return *mDotsGrid;
    }

    Scheduler& GetScheduler() const
    {
        return *mScheduler;
    }

    PacmanController& GetPacmanController() const
    {
        return *mPacmanController;
    }

    AIController& GetAIController() const
    {
        return *mAIController;
    }

    SharedDataManager& GetSharedDataManager() const
    {
        return *mSharedDataManager;
    }

private:

    void InitActionsAndTriggers();

    std::unique_ptr<GameLoader>        mLoader;
    std::unique_ptr<Map>               mMap;
    std::unique_ptr<DotsGrid>          mDotsGrid;
    std::unique_ptr<Scheduler>         mScheduler;
    std::unique_ptr<PacmanController>  mPacmanController;
    std::unique_ptr<AIController>      mAIController;
    std::unique_ptr<SharedDataManager> mSharedDataManager;
};

Game& GetGame();

} // Pacman namespace