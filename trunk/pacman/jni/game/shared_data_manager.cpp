#include "shared_data_manager.h"

#include "shared_data_context.h"
#include "game.h"
#include "map.h"
#include "actor.h"
#include "pacman_controller.h"

namespace Pacman {

SharedDataManager::SharedDataManager()
                 : mContext(new SharedDataContext())
{
}

SharedDataManager::~SharedDataManager()
{
}

void SharedDataManager::Reset()
{
    mContext->Reset();
}

CellIndexArray SharedDataManager::GetPacmanCells()
{
    static const std::string kKey = "pacmanCells";

    if (mContext->HasValue(kKey))
    {
        return mContext->GetValue<CellIndexArray>(kKey);
    }
    else
    {
        Game& game = GetGame();
        PacmanController& pacmanController = game.GetPacmanController();
        Map& map = game.GetMap();
        const CellIndexArray cells = map.FindCells(pacmanController.GetActor()->GetRegion());
        mContext->SetValue(kKey, cells);
        return cells;
    }
}

} // Pacman namespace