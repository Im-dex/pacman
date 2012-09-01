#include "dots_grid.h"

namespace Pacman {

DotsGrid::DotsGrid(const std::vector<DotType>& dotsInfo, std::shared_ptr<Map> map)
        : mDots(dotsInfo),
          mMap(map)
{
}

} // Pacman namespace