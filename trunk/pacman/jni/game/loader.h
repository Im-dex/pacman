#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game_typedefs.h"

namespace Pacman {

class Map;
class DotsGrid;
class SpriteSheet;
class PacmanActor;
class GhostActor;

class GameLoader
{
public:

    GameLoader() = default;
    GameLoader(const GameLoader&) = delete;
    ~GameLoader() = default;

    GameLoader& operator= (const GameLoader&) = delete;

    std::shared_ptr<Map> LoadMap(const std::string& fileName, const uint16_t cellSize);

    std::shared_ptr<DotsGrid> MakeDotsGrid(const std::weak_ptr<Map> mapPtr, const std::weak_ptr<SpriteSheet> spritesheetPtr);

    std::shared_ptr<PacmanActor> LoadPacmanActor(const std::string& fileName, const uint16_t actorSize,
                                                 const std::weak_ptr<SpriteSheet> spriteSheet, const std::shared_ptr<Map> map) const;

    std::shared_ptr<GhostActor> LoadGhostActor(const std::string& fileName, const uint16_t actorSize,
                                               const std::weak_ptr<SpriteSheet> spriteSheet, const std::shared_ptr<Map> map) const;
private:

    std::vector<DotType> mDotsInfo;
};

} // Pacman namespace