#pragma once

#include "game_typedefs.h"

namespace Pacman {

class IActorController
{
public:

    virtual void OnDirectionChanged(const MoveDirection newDirection) = 0;

    virtual void OnTargetAchieved() = 0;
};

} // Pacman namespace