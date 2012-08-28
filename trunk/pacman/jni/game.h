#pragma once

#include "engine.h"

#include <cstdint>
#include <memory>

namespace Pacman {

class SceneNode;

class Game : public IEngineListener
{
public:

	Game() = default;
	Game(const Game&) = delete;
	~Game() = default;

	Game& operator= (const Game&) = delete;

	virtual void OnLoad();

	virtual void OnUnload();

	virtual void OnUpdate(const uint64_t dt);

private:

	std::shared_ptr<SceneNode> mPacmanNode;
};

} // Pacman namespace