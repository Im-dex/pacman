#include "ghosts_factory.h"

#include "math.h"
#include "game.h"
#include "common.h"
#include "ghost.h"
#include "actor.h"
#include "error.h"
#include "loader.h"
#include "map.h"
#include "pacman_controller.h"
#include "ai_controller.h"
#include "shared_data_manager.h"

namespace Pacman {

static CellIndex FindWithOffset(const CellIndex& cellIndex, const MoveDirection direction,
                                const CellIndex::value_t offset)
{
    switch (direction)
    {
    case MoveDirection::Left:
        if (GetColumn(cellIndex) < offset)
            return CellIndex(GetRow(cellIndex), 0);
        else
            return CellIndex(GetRow(cellIndex), GetColumn(cellIndex) - offset);
    case MoveDirection::Right:
        return CellIndex(GetRow(cellIndex), GetColumn(cellIndex) + offset);
    case MoveDirection::Up:
        if (GetRow(cellIndex) < offset)
            return CellIndex(0, GetColumn(cellIndex));
        else
            return CellIndex(GetRow(cellIndex) - offset, GetColumn(cellIndex));
    case MoveDirection::Down:
        return CellIndex(GetRow(cellIndex) + offset, GetColumn(cellIndex));
    default:
        return cellIndex;
    }
}

class Blinky : public Ghost
{
public:

    Blinky(const std::shared_ptr<Actor>& actor, const Size size,
           const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
        : Ghost(actor, size, spriteSheetPtr, GhostState::Chase,
                "blinky_left", "blinky_right", "blinky_top", "blinky_bottom")
    {
    }

    Blinky(const Blinky&) = delete;
    ~Blinky() = default;

    Blinky& operator= (const Blinky&) = delete;

    // target is pacman cell
    virtual CellIndex SelectTargetCell() const
    {
        Game& game = GetGame();
        const std::shared_ptr<Actor> pacman = game.GetPacmanController().GetActor();
        return SelectNearestCell(game.GetSharedDataManager().GetPacmanCells(), pacman->GetDirection());
    }
};

class Pinky : public Ghost
{
public:

    Pinky(const std::shared_ptr<Actor>& actor, const Size size,
          const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
      : Ghost(actor, size, spriteSheetPtr, GhostState::Chase,
        "pinky_left", "pinky_right", "pinky_top", "pinky_bottom")
    {
    }

    Pinky(const Pinky&) = delete;
    ~Pinky() = default;

    Pinky& operator= (const Pinky&) = delete;

    // target is pacman cell + 4 cells by pacman direction
    virtual CellIndex SelectTargetCell() const
    {
        static const CellIndex::value_t kOffset = 4;

        Game& game = GetGame();
        const std::shared_ptr<Actor> pacman = game.GetPacmanController().GetActor();
        const CellIndex pacmanCell = SelectNearestCell(game.GetSharedDataManager().GetPacmanCells(), pacman->GetDirection());
        return FindWithOffset(pacmanCell, pacman->GetDirection(), kOffset);
    }
};
//#include "log.h"
class Inky : public Ghost
{
public:

    Inky(const std::shared_ptr<Actor>& actor, const Size size,
         const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
      : Ghost(actor, size, spriteSheetPtr, GhostState::Wait,
        "inky_left", "inky_right", "inky_top", "inky_bottom")
    {
        // wait while 30 dots not eaten
    }

    Inky(const Inky&) = delete;
    ~Inky() = default;

    Inky& operator= (const Inky&) = delete;

    // target is double vector from blinky to (pacman cell + 2 cells by pacman direction)
    virtual CellIndex SelectTargetCell() const
    {
        static const CellIndex::value_t kOffset = 2;

        Game& game = GetGame();
        Map& map = game.GetMap();
        const std::shared_ptr<Actor> pacman = game.GetPacmanController().GetActor();
        const std::shared_ptr<Actor> blinky = game.GetAIController().GetGhost(GhostId::Blinky).GetActor();
        const CellIndex pacmanCell = SelectNearestCell(game.GetSharedDataManager().GetPacmanCells(), pacman->GetDirection());
        const CellIndex blinkyCell = SelectNearestCell(game.GetSharedDataManager().GetGhostCells(GhostId::Blinky), blinky->GetDirection());
        const CellIndex pacmanOffsetCell = FindWithOffset(pacmanCell, pacman->GetDirection(), kOffset);
        
        const CellIndex::value_t blinkyRow = GetRow(blinkyCell);
        const CellIndex::value_t blinkyColumn = GetColumn(blinkyCell);
        const CellIndex::value_t pacmanOffsetRow = GetRow(pacmanOffsetCell);
        const CellIndex::value_t pacmanOffsetColumn = GetColumn(pacmanOffsetCell);

        const CellIndex::value_t rowDiff = (std::max(blinkyRow, pacmanOffsetRow) - std::min(blinkyRow, pacmanOffsetRow)) * 2;
        const CellIndex::value_t columnDiff = (std::max(blinkyColumn, pacmanOffsetColumn) - std::min(blinkyColumn, pacmanOffsetColumn)) * 2;

        const CellIndex::value_t resultRow = CalcIndexValue(blinkyRow, pacmanOffsetRow, rowDiff);
        const CellIndex::value_t resultColumn = CalcIndexValue(blinkyColumn, pacmanOffsetColumn, columnDiff);

        //LogI("row: %u, col: %u", resultRow, resultColumn);
        return CellIndex(resultRow, resultColumn);
    }

private:

    CellIndex::value_t CalcIndexValue(const CellIndex::value_t blinkyValue, const CellIndex::value_t pacmanOffsetValue,
                                      const CellIndex::value_t diff) const
    {
        if (blinkyValue >= pacmanOffsetValue)
            return blinkyValue + diff;

        // else
        return (blinkyValue >= diff) ? (blinkyValue - diff) : 0;
    }
};

class Clyde : public Ghost
{
public:

    Clyde(const std::shared_ptr<Actor>& actor, const Size size,
          const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
      : Ghost(actor, size, spriteSheetPtr, GhostState::Wait,
        "clyde_left", "clyde_right", "clyde_top", "clyde_bottom")
    {
        // wait while 1/3 of dots not eaten
    }

    Clyde(const Clyde&) = delete;
    ~Clyde() = default;

    Clyde& operator= (const Clyde&) = delete;

    // if distance to pacman greater than 8 cells - as blinky, scatter target otherwise
    virtual CellIndex SelectTargetCell() const
    {
        static const float kMaxDistance = 8.0f;

        Game& game = GetGame();
        const std::shared_ptr<Actor> pacman = game.GetPacmanController().GetActor();
        const std::shared_ptr<Actor> clyde = game.GetAIController().GetGhost(GhostId::Clyde).GetActor();
        const CellIndex pacmanCell = SelectNearestCell(game.GetSharedDataManager().GetPacmanCells(), pacman->GetDirection());
        const CellIndex currentCell = SelectNearestCell(game.GetSharedDataManager().GetGhostCells(GhostId::Clyde), clyde->GetDirection());

        const Math::Vector2f currentPoint(static_cast<float>(GetRow(currentCell)), static_cast<float>(GetColumn(currentCell)));
        const Math::Vector2f pacmanPoint(static_cast<float>(GetRow(pacmanCell)), static_cast<float>(GetColumn(pacmanCell)));
        const float distance = (pacmanPoint - currentPoint).Length();
        if (Math::Comparator<float>::Greater(distance, kMaxDistance))
            return pacmanCell;
        else
            return game.GetAIController().GetScatterTarget(GhostId::Clyde);
    }
};

std::shared_ptr<Ghost> GhostsFactory::CreateGhost(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr,
                                                 const GhostId ghostId) const
{
    GameLoader& loader = GetGame().GetLoader();

    switch (ghostId)
    {
    case GhostId::Blinky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("blinky.json", actorSize, nullptr);
            return std::make_shared<Blinky>(actor, actorSize, spriteSheetPtr);
        }
    case GhostId::Pinky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("pinky.json", actorSize, nullptr);
            return std::make_shared<Pinky>(actor, actorSize, spriteSheetPtr);
        }
    case GhostId::Inky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("inky.json", actorSize, nullptr);
            return std::make_shared<Inky>(actor, actorSize, spriteSheetPtr);
        }
    case GhostId::Clyde:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("clyde.json", actorSize, nullptr);
            return std::make_shared<Clyde>(actor, actorSize, spriteSheetPtr);
        }
    default:
        PACMAN_CHECK_ERROR2(false, "Wrong ghost id");
    }
}

} // Pacman namespace
