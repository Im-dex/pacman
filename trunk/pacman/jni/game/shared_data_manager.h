#pragma once

#include <memory>

#include "game_typedefs.h"

namespace Pacman {

class SharedDataContext;

class SharedDataManager
{
public:

    SharedDataManager();
    SharedDataManager(const SharedDataManager&) = delete;
    ~SharedDataManager();

    SharedDataManager& operator= (const SharedDataManager&) = delete;

    void Reset();

    CellIndexArray GetPacmanCells();

private:

    std::unique_ptr<SharedDataContext> mContext;
};

} // Pacman namespace