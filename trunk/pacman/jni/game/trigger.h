#pragma once

#include <functional>

namespace Pacman {

enum class ActionResult
{
    None,
    Unregister
};

typedef std::function<ActionResult()> Action;
typedef std::function<bool()> Condition;

class Trigger
{
public:

    Trigger() = delete;
    Trigger(const Condition& condition, const Action& action);
    Trigger(const Trigger&) = default;
    ~Trigger() = default;

    Trigger& operator= (const Trigger&) = default;

    ActionResult Update();

private:

    Condition mCondition;
    Action    mAction;
};

} // Pacman namespace