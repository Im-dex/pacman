#include "unique_id.h"

namespace Pacman {

uid_t UniqueIdProvider::gIdCounter = 1;
const uid_t UniqueIdProvider::kNull = 0;

} // Pacman namespace
