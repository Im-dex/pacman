#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game_typedefs.h"

namespace Pacman {

class Map;
class SpriteSheet;
class IDrawable;
class DotsGrid;
class Actor;
class IActorListener;

class GameLoader
{
public:

    GameLoader() = default;
    GameLoader(const GameLoader&) = delete;
    ~GameLoader() = default;

    GameLoader& operator= (const GameLoader&) = delete;

    std::shared_ptr<Map> LoadMap(const std::string& fileName, const Size cellSize);

    std::shared_ptr<DotsGrid> MakeDotsGrid(const std::weak_ptr<Map>& mapPtr, const std::weak_ptr<SpriteSheet>& spritesheetPtr);

    std::shared_ptr<Actor> LoadActor(const std::string& fileName, const Size actorSize,
                                     const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<Map>& map,
                                     const std::shared_ptr<IActorListener>& listener) const;
private:

    std::vector<DotType> mDotsInfo;
};

} // Pacman namespace