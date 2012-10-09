#pragma once

#include <cstdint>
#include <vector>

#include "engine_typedefs.h"
#include "math/vector2.h"

namespace Pacman {

typedef uint16_t                Speed;
typedef Math::Vector2<uint16_t> CellIndex;
typedef std::vector<CellIndex>  CellIndexArray;

static FORCEINLINE CellIndex::value_t GetRow(const CellIndex& cell)
{
    return cell.GetX();
}

static FORCEINLINE CellIndex::value_t GetColumn(const CellIndex& cell)
{
    return cell.GetY();
}

static FORCEINLINE CellIndex::value_t SetRow(CellIndex& cell, const CellIndex::value_t value)
{
    cell.SetX(value);
}

static FORCEINLINE CellIndex::value_t SetColumn(CellIndex& cell, const CellIndex::value_t value)
{
    cell.SetY(value);
}

//================================================

enum class MapCellType : uint8_t
{
    Empty = 0,
    Wall  = 1,
    Door  = 2,
    Space = 3 // out of map area
};

//================================================

enum class DotType : uint8_t
{
    None  = 0,
    Small = 4,
    Big   = 5
};

//================================================

enum class MoveDirection : uint8_t
{
    None  = 0,
    Left  = 1,
    Right = 2,
    Up    = 3,
    Down  = 4
};

//================================================

enum class GhostState
{
    Wait,
    LeaveHouse,
    Chase,
    Scatter,
    Frightened
};

//================================================

struct Neighbor
{
    MapCellType   mCellType;
    MoveDirection mDirection;
};

} // Pacman namespace