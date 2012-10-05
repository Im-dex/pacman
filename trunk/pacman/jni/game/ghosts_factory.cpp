#include "ghosts_factory.h"

#include "ghost.h"
#include "error.h"
#include "loader.h"

namespace Pacman {

const size_t GhostsFactory::kBlinky = 0;
const size_t GhostsFactory::kPinky = 1;
const size_t GhostsFactory::kInky = 2;
const size_t GhostsFactory::kClyde = 3;

class Blinky : public Ghost
{
public:

    Blinky(const std::shared_ptr<Actor>& actor, const Size size,
           const std::weak_ptr<SpriteSheet>& spriteSheetPtr)
        : Ghost(actor, size, spriteSheetPtr,
                "blinky_left", "blinky_right", "blinky_top", "blinky_bottom")
    {
    }

    Blinky(const Blinky&) = delete;
    ~Blinky() = default;

    Blinky& operator= (const Blinky&) = delete;

    virtual CellIndex SelectTargetCell() const
    {
        return CellIndex::kZero;
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
    }

    Pinky(const Pinky&) = delete;
    ~Pinky() = default;

    Pinky& operator= (const Pinky&) = delete;

    virtual CellIndex SelectTargetCell() const
    {
        return CellIndex::kZero;
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
    }

    Inky(const Inky&) = delete;
    ~Inky() = default;

    Inky& operator= (const Inky&) = delete;

    virtual CellIndex SelectTargetCell() const
    {
        return CellIndex::kZero;
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
    }

    Clyde(const Clyde&) = delete;
    ~Clyde() = default;

    Clyde& operator= (const Clyde&) = delete;

    virtual CellIndex SelectTargetCell() const
    {
        return CellIndex::kZero;
    }
};

std::shared_ptr<Ghost> GhostsFactory::CreateGhost(const GameLoader& loader, const Size actorSize,
                                                 const std::shared_ptr<Map>& map, 
                                                 const std::weak_ptr<SpriteSheet>& spriteSheetPtr,
                                                 const size_t ghostId) const
{
    switch (ghostId)
    {
    case kBlinky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("blinky.json", actorSize, nullptr, map);
            return std::make_shared<Blinky>(actor, actorSize, spriteSheetPtr);
        }
    case kPinky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("pinky.json", actorSize, nullptr, map);
            return std::make_shared<Pinky>(actor, actorSize, spriteSheetPtr);
        }
    case kInky:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("inky.json", actorSize, nullptr, map);
            return std::make_shared<Inky>(actor, actorSize, spriteSheetPtr);
        }
    case kClyde:
        {
            const std::shared_ptr<Actor> actor = loader.LoadActor("clyde.json", actorSize, nullptr, map);
            return std::make_shared<Clyde>(actor, actorSize, spriteSheetPtr);
        }
    default:
        PACMAN_CHECK_ERROR2(false, ErrorCode::BadArgument, "Wrong ghost id");
    }
}

} // Pacman namespace