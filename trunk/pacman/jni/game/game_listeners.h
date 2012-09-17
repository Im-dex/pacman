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

static const size_t kMoveDirectionsCount = 5;

class IActorListener
{
public:

    virtual void OnDirectionChanged(const std::weak_ptr<Actor> actorPtr, const MoveDirection newDirection) = 0;

    virtual void OnTargetAchieved(const std::weak_ptr<Actor> actorPtr) = 0;
};

} // Pacman namespace