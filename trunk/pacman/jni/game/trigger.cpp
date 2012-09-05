#include "trigger.h"
#include "scheduler.h"

namespace Pacman {

Trigger::Trigger(const Condition& condition, const Action& action)
       : mCondition(condition),
         mAction(action)
{
}

ActionResult Trigger::Update()
{
    if (mCondition())
    {
        return mAction();
    }

    return ActionResult::None;
}

} // Pacman namespace