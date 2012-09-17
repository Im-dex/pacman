#pragma once

#include <memory>

#include "engine_listeners.h"
#include "scheduler.h"
#include "map.h"
#include "dots_grid.h"
#include "pacman_controller.h"

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

    void InitActionsAndTriggers(const std::shared_ptr<Map> map, const std::shared_ptr<DotsGrid> dots);

    std::unique_ptr<Scheduler>        mScheduler;
    std::unique_ptr<PacmanController> mPacmanController;
};

} // Pacman namespace