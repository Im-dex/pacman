#include "ghosts_factory.h"

#include "game.h"
#include "common.h"
#include "ghost.h"
#include "actor.h"
#include "error.h"
#include "loader.h"
#include "pacman_controller.h"
#include "shared_data_manager.h"
#include "math.h"

namespace Pacman {

const size_t GhostsFactory::kBlinky = 0;
const size_t GhostsFactory::kPinky = 1;
const size_t GhostsFactory::kInky = 2;
const size_t GhostsFactory::kClyde = 3;

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
        : Ghost(actor, size, spriteSheetPtr,
                "blinky_left", "blinky_right", "blinky_top", "blinky_bottom")
    {
        mState = GhostState::Chase;
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
      : Ghost(actor, size, spriteSheetPtr,
        "pinky_left", "pinky_right", "pinky_top", "pinky_bottom")
    {
        mState = GhostState::Chase;
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

class Inky : public Ghost
{
public:

    Inky(const std::shared_ptr<Actor>& actor, const Size size,
         const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
      : Ghost(actor, size, spriteSheetPtr,
        "inky_left", "inky_right", "inky_top", "inky_bottom")
    {
        mState = GhostState::Wait; // while 30 dots not eaten
    }

    Inky(const Inky&) = delete;
    ~Inky() = default;

    Inky& operator= (const Inky&) = delete;

    // target is double vector from blinky to pacman cell + 2 cells by pacman direction
    virtual CellIndex SelectTargetCell() const
    {
        //static const CellIndex::value_t kOffset = 2;
        //const CellIndex controlCell = FindWithOffset(pacmanCell, pacmanDirection, kOffset);
        //const CellIndex::value_t rowIncrement = (GetRow(controlCell) > GetRow(blinkyCell)) ? GetRow(controlCell) - GetRow(blinkyCell)
        //                                                                                   : GetRow(blinkyCell) - GetRow(controlCell);
        //const CellIndex::value_t columnIncrement = (GetColumn(controlCell) > GetColumn(blinkyCell)) ? GetColumn(controlCell) - GetColumn(blinkyCell)
        //                                                                                            : GetColumn(blinkyCell) - GetColumn(controlCell);
        //return controlCell + CellIndex(rowIncrement, columnIncrement);
        Game& game = GetGame();
        const std::shared_ptr<Actor> pacman = game.GetPacmanController().GetActor();
        return SelectNearestCell(game.GetSharedDataManager().GetPacmanCells(), pacman->GetDirection());
    }
};

class Clyde : public Ghost
{
public:

    Clyde(const std::shared_ptr<Actor>& actor, const Size size,
          const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
      : Ghost(actor, size, spriteSheetPtr,
        "clyde_left", "clyde_right", "clyde_top", "clyde_bottom")
    {
        mState = GhostState::Wait; // while 1/3 of dots not eaten
    }

    Clyde(const Clyde&) = delete;
    ~Clyde() = default;

    Clyde& operator= (const Clyde&) = delete;

    // if distance to pacman greater than 8 cells - as blinky, scatter target otherwise
    virtual CellIndex SelectTargetCell() const
    {
        /*static const float kMaxDistance = 8.0f;
        const CellIndex currentCell = this->GetActor()->GetNearestCellIndex();
        const Math::Vector2f currentPoint(static_cast<float>(GetRow(currentCell)), static_cast<float>(GetColumn(currentCell)));
        const Math::Vector2f pacmanPoint(static_cast<float>(GetRow(pacmanCell)), static_cast<float>(GetColumn(pacmanCell)));
        const float distance = (pacmanPoint - currentPoint).Length();
        if (Math::Comparator<float>::Greater(distance, kMaxDistance))
        return pacmanCell;
        else
        return scatterTarget;*/
        Game& game = GetGame();
        const std::shared_ptr<Actor> pacman = game.GetPacmanController().GetActor();
        return SelectNearestCell(game.GetSharedDataManager().GetPacmanCells(), pacman->GetDirection());
    }
};

std::shared_ptr<Ghost> GhostsFactory::CreateGhost(const Size actorSize, const std::weak_ptr<SpriteSheet>& spriteSheetPtr,
                                                 const size_t ghostId) const
{
    GameLoader& loader = GetGame().GetLoader();

    switch (ghostId)
    {
    case kBlinky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("blinky.json", actorSize, nullptr);
            return std::make_shared<Blinky>(actor, actorSize, spriteSheetPtr);
        }
    case kPinky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("pinky.json", actorSize, nullptr);
            return std::make_shared<Pinky>(actor, actorSize, spriteSheetPtr);
        }
    case kInky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("inky.json", actorSize, nullptr);
            return std::make_shared<Inky>(actor, actorSize, spriteSheetPtr);
        }
    case kClyde:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("clyde.json", actorSize, nullptr);
            return std::make_shared<Clyde>(actor, actorSize, spriteSheetPtr);
        }
    default:
        PACMAN_CHECK_ERROR2(false, ErrorCode::BadArgument, "Wrong ghost id");
    }
}

} // Pacman namespace