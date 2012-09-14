#pragma once

#include "game_typedefs.h"
#include "actor.h"

namespace Pacman {

class Map;
class SpriteSheet;
class FrameAnimator;

enum class PacmanMoveDirection : uint8_t
{
    None  = 0,
    Left  = 1,
    Right = 2,
    Up    = 3,
    Down  = 4
};

static const size_t kPacmanMoveDirectionCount = 5;

class PacmanActor : public Actor
{
public:

    PacmanActor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
                const uint16_t cellSize, const uint64_t animationFrameDuration,
                const PacmanMoveDirection startDirection, const std::weak_ptr<SpriteSheet> spriteSheet,
                const std::shared_ptr<Map> map);

    PacmanActor(const PacmanActor&) = delete;
    ~PacmanActor() = default;

    PacmanActor& operator= (const PacmanActor&) = delete;

    void ChangeDirection(const PacmanMoveDirection direction);

    void Update(const uint64_t dt);

private:

    CellIndex FindMaxAvailableCell(const CellIndex& curCellIndex, const PacmanMoveDirection direction) const;

    const SpritePosition           mSpriteCenterOffset;
    const std::shared_ptr<Map>     mMap;
    std::shared_ptr<FrameAnimator> mAnimator;
};

} // Pacman namespace