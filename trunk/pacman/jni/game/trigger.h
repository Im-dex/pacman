#pragma once

#include <functional>

#include "scheduler_context.h"

namespace Pacman {

enum class ActionResult
{
    None,
    Unregister
};

typedef std::function<ActionResult(SchedulerContext& context)> Action;
typedef std::function<bool(SchedulerContext& context)> Condition;

class Trigger
{
public:

    Trigger() = delete;
    Trigger(const Condition& condition, const Action& action);
    Trigger(const Trigger&) = default;
    ~Trigger() = default;

    Trigger& operator= (const Trigger&) = default;

    ActionResult Update(SchedulerContext& context);

private:

    Condition mCondition;
    Action    mAction;
};

} // Pacman namespace