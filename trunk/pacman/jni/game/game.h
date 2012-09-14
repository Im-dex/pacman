#pragma once

#include <memory>

#include "engine_listeners.h"
#include "loader.h"
#include "scheduler.h"
#include "pacman.h"
#include "map.h"

namespace Pacman {

class SceneNode;
class FrameAnimator;

class Game : public IEngineListener, public IGestureListener
{
public:

	Game() = default;
	Game(const Game&) = delete;
	~Game() = default;

	Game& operator= (const Game&) = delete;

	virtual void OnLoad();

	virtual void OnUnload();

	virtual void OnUpdate(const uint64_t dt);

    virtual void OnGesture(const GestureType gestureType);

private:

    GameLoader mLoader;
    Scheduler mScheduler;

    std::shared_ptr<Map> mMap;
	std::shared_ptr<PacmanActor> mPacman;
};

} // Pacman namespace