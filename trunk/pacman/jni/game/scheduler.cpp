#include "scheduler.h"

namespace Pacman {

void Scheduler::UpdateEvents(const uint64_t dt)
{
    for (EventData& eventData : mEvents)
    {
        eventData.mElapsedInterval += dt;
        if (eventData.mElapsedInterval >= eventData.mDelay)
        {
            eventData.mElapsedInterval = 0;
            const ActionResult result = eventData.mAction();
            if ((!eventData.mRepeatable) || (result == ActionResult::Unregister))
                UnregisterEvent(eventData.mActionId);
        }
    }

    CleanupEvents();
}

void Scheduler::UpdateTriggers()
{
    for (TriggerData& triggerData : mTriggers)
    {
        const ActionResult result = triggerData.mAction();
        if (result == ActionResult::Unregister)
            UnregisterTrigger(triggerData.mActionId);
    }

    CleanupTriggers();
}

void Scheduler::RegisterEvent(const Action& action, const uint64_t delay, const bool repeatable)
{
    static uint64_t actionIdCounter = 0;
    mEvents.push_back(EventData { delay, 0, actionIdCounter++, repeatable, action });
}

void Scheduler::RegisterTrigger(const Action& action)
{
    static uint64_t actionIdCounter = 0;
    mTriggers.push_back(TriggerData { actionIdCounter++, action });
}

void Scheduler::UnregisterEvent(const uint64_t actionId)
{
    mUnregisteredEvents.push_back(actionId);
}

void Scheduler::UnregisterTrigger(const uint64_t actionId)
{
    mUnregisteredTriggers.push_back(actionId);
}

void Scheduler::CleanupEvents()
{
    for (const uint64_t actionId : mUnregisteredEvents)
    {
        mEvents.remove_if([actionId](const EventData& eventData)
        {
            return eventData.mActionId == actionId;
        });    
    }

    mUnregisteredEvents.clear();
}

void Scheduler::CleanupTriggers()
{
    for (const uint64_t actionId : mUnregisteredTriggers)
    {
        mTriggers.remove_if([actionId](const TriggerData& triggerData)
        {
            return triggerData.mActionId == actionId;
        });    
    }

    mUnregisteredTriggers.clear();
}

} // Pacman namespace