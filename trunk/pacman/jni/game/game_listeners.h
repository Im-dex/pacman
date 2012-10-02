#pragma once

#include <memory>

namespace Pacman {

class Actor;

enum class MoveDirection : uint8_t
{
    None  = 0,
    Left  = 1,
    Right = 2,
    Up    = 3,
    Down  = 4
};

class IActorListener
{
public:

    virtual ~IActorListener() {}

    virtual void OnDirectionChanged(Actor& actor, const MoveDirection newDirection) = 0;

    virtual void OnTargetAchieved(Actor& actor) = 0;
};

} // Pacman namespace