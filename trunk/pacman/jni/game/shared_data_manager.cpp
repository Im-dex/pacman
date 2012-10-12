#include "shared_data_manager.h"

#include "shared_data_context.h"
#include "game.h"
#include "map.h"
#include "actor.h"
#include "pacman_controller.h"
#include "ai_controller.h"
#include "ghosts_factory.h"
#include "ghost.h"

namespace Pacman {

static FORCEINLINE std::string GetGhostKeyName(const GhostId ghostId)
{
    switch (ghostId)
    {
    case GhostId::Blinky:
        return "blinkyCells";
    case GhostId::Pinky:
        return "inkyCells";
    case GhostId::Inky:
        return "pinkyCells";
    case GhostId::Clyde:
        return "clydeCells";
    }
}

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
    return GetActorCells(GetGame().GetPacmanController().GetActor(), kKey);
}

CellIndexArray SharedDataManager::GetGhostCells(const GhostId ghostId)
{
    return GetActorCells(GetGame().GetAIController().GetGhost(ghostId).GetActor(), GetGhostKeyName(ghostId));
}

CellIndexArray SharedDataManager::GetActorCells(const std::shared_ptr<Actor>& actor, const std::string& keyName)
{
    if (mContext->HasValue(keyName))
    {
        return mContext->GetValue<CellIndexArray>(keyName);
    }
    else
    {
        Game& game = GetGame();
        Map& map = game.GetMap();
        const CellIndexArray cells = map.FindCells(actor->GetRegion());
        mContext->SetValue(keyName, cells);
        return cells;
    }
}

} // Pacman namespace