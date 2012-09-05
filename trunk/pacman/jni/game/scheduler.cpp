#include "scheduler.h"

namespace Pacman {

void Scheduler::Update(const uint64_t dt)
{
    // update actions
    for (ActionData actionData : mActions)
    {
        actionData.mElapsedInterval += dt;
        if (actionData.mElapsedInterval >= actionData.mDelay)
        {
            ActionResult result = (*(actionData.mAction))();
            if ((!actionData.mRepeatable) || (result == ActionResult::Unregister))
                UnregisterAction(actionData.mAction);
        }
    }

    // update triggers
    for (std::shared_ptr<Trigger>& trigger : mTriggers)
    {
        ActionResult result = trigger->Update();
        if (result == ActionResult::Unregister)
            UnregisterTrigger(trigger);
    }

    // cleanup actions and triggers
    Cleanup();
}

void Scheduler::RegisterAction(const std::shared_ptr<Action> action, const uint64_t delay, const bool repeatable)
{
    mActions.push_back(ActionData { delay, 0, repeatable, action });
}

void Scheduler::RegisterTrigger(const std::shared_ptr<Trigger> trigger)
{
    mTriggers.push_back(trigger);
}

void Scheduler::UnregisterAction(const std::shared_ptr<Action>& action)
{
    mUnregisteredActions.push_back(action);
}

void Scheduler::UnregisterTrigger(const std::shared_ptr<Trigger>& trigger)
{
    mUnregisteredTriggers.push_back(trigger);
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

    for (const std::shared_ptr<Trigger>& trigger : mUnregisteredTriggers)
    {
        mTriggers.remove(trigger);
    }

    mUnregisteredActions.clear();
    mUnregisteredTriggers.clear();
}

} // Pacman namespace