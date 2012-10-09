#include "common.h"

namespace Pacman {

CellIndex SelectNearestCell(const CellIndexArray& currentCellsIndices, const MoveDirection direction)
{
    CellIndex result = currentCellsIndices[0];

    for (size_t i = 1; i < currentCellsIndices.size(); i++)
    {
        const CellIndex& cellIndex = currentCellsIndices[i];
        switch (direction)
        {
        case MoveDirection::Left:
            if (GetColumn(cellIndex) < GetColumn(result))
                result = cellIndex;
            break;
        case MoveDirection::Right:
            if (GetColumn(cellIndex) > GetColumn(result))
                result = cellIndex;
            break;
        case MoveDirection::Up:
            if (GetRow(cellIndex) < GetRow(result))
                result = cellIndex;
            break;
        case MoveDirection::Down:
            if (GetRow(cellIndex) > GetRow(result))
                result = cellIndex;
            break;
        }
    }

    return result;
}

} // Pacman namespace