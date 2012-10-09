#include "scheduler.h"

namespace Pacman {

void Scheduler::Update(const uint64_t dt)
{
    // update actions
    for (ActionData& actionData : mActions)
    {
        actionData.mElapsedInterval += dt;
        if (actionData.mElapsedInterval >= actionData.mDelay)
        {
            actionData.mElapsedInterval = 0;
            const ActionResult result = (*(actionData.mAction))();
            if ((!actionData.mRepeatable) || (result == ActionResult::Unregister))
                UnregisterAction(actionData.mAction);
        }
    }

    // cleanup context, actions and triggers
    Cleanup();
}

void Scheduler::RegisterAction(const std::shared_ptr<Action>& action, const uint64_t delay, const bool repeatable)
{
    mActions.push_back(ActionData { delay, 0, repeatable, action });
}

void Scheduler::UnregisterAction(const std::shared_ptr<Action>& action)
{
    mUnregisteredActions.push_back(action);
}

void Scheduler::Cleanup()
{
    for (const std::shared_ptr<Action>& action : mUnregisteredActions)
    {
        mActions.remove_if([&action](const ActionData& actionData)
        {
            return actionData.mAction == action;
        });    
    }

    mUnregisteredActions.clear();
}

} // Pacman namespace