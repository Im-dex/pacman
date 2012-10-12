#pragma once

#include "game_typedefs.h"

namespace Pacman {

// if cells count greater than 1 select nearest cell for the current direction
// (for example: if direction is left, select one of the most left placed cells)
CellIndex SelectNearestCell(const CellIndexArray& currentCellsIndices, const MoveDirection direction);

static FORCEINLINE MoveDirection GetBackDirection(const MoveDirection direction)
{
    switch (direction)
    {
    case MoveDirection::Left:
        return MoveDirection::Right;
    case MoveDirection::Right:
        return MoveDirection::Left;
    case MoveDirection::Up:
        return MoveDirection::Down;
    case MoveDirection::Down:
        return MoveDirection::Up;
    default:
        return MoveDirection::None;
    }
}

static FORCEINLINE CellIndex GetNext(const CellIndex& current, const MoveDirection direction)
{
    switch (direction)
    {
    case MoveDirection::Left:
        return CellIndex(GetRow(current), GetColumn(current) - 1);
    case MoveDirection::Right:
        return CellIndex(GetRow(current), GetColumn(current) + 1);
    case MoveDirection::Up:
        return CellIndex(GetRow(current) - 1, GetColumn(current));
    case MoveDirection::Down:
        return CellIndex(GetRow(current) + 1, GetColumn(current));
    default:
        return current;
    }
}

static FORCEINLINE Position GetFuturePosition(const Position& current, const MoveDirection direction,
                                              const Size wayLength)
{
    switch (direction)
    {
    case MoveDirection::Left:
        return Position(current.GetX() - wayLength, current.GetY());
    case MoveDirection::Right:
        return Position(current.GetX() + wayLength, current.GetY());
    case MoveDirection::Up:
        return Position(current.GetX(), current.GetY() - wayLength);
    case MoveDirection::Down:
        return Position(current.GetX(), current.GetY() + wayLength);
    default:
        return current;
    }
}

bool IsDifferent(const CellIndexArray& first, const CellIndexArray& second);

} // Pacman namespace