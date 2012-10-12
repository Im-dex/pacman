#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game_forwdecl.h"
#include "game_typedefs.h"

namespace Pacman {

class GameLoader
{
public:

    GameLoader() = default;
    GameLoader(const GameLoader&) = delete;
    ~GameLoader() = default;

    GameLoader& operator= (const GameLoader&) = delete;

    std::unique_ptr<Map> LoadMap(const std::string& fileName, const Size cellSize);

    std::unique_ptr<DotsGrid> MakeDotsGrid(const std::weak_ptr<SpriteSheet>& spritesheetPtr);

    std::shared_ptr<Actor> LoadActor(const std::string& fileName, const Size actorSize,
                                     const std::shared_ptr<IDrawable>& drawable) const;

    AIInfo LoadAIInfo(const std::string& fileName) const;

private:

    std::vector<DotType> mDotsInfo;
};

} // Pacman namespace