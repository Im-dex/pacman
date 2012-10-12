#pragma once

#include <memory>

#include "game_forwdecl.h"
#include "game_typedefs.h"

namespace Pacman {

class SharedDataManager
{
public:

    SharedDataManager();
    SharedDataManager(const SharedDataManager&) = delete;
    ~SharedDataManager();

    SharedDataManager& operator= (const SharedDataManager&) = delete;

    void Reset();

    CellIndexArray GetPacmanCells();

    CellIndexArray GetGhostCells(const GhostId ghostId);

private:

    CellIndexArray GetActorCells(const std::shared_ptr<Actor>& actor, const std::string& keyName);

    std::unique_ptr<SharedDataContext> mContext;
};

} // Pacman namespace