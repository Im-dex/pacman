#include "trigger.h"
#include "scheduler.h"

namespace Pacman {

Trigger::Trigger(const Condition& condition, const Action& action)
       : mCondition(condition),
         mAction(action)
{
}

ActionResult Trigger::Update(SchedulerContext& context)
{
    if (mCondition(context))
    {
        return mAction(context);
    }

    return ActionResult::None;
}

} // Pacman namespace