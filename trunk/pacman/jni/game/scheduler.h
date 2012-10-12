#pragma once

#include <cstdint>
#include <functional>
#include <list>

namespace Pacman {

enum class ActionResult
{
    None,
    Unregister
};

typedef std::function<ActionResult()> Action;

class Scheduler
{
public:

    Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    ~Scheduler() = default;

    Scheduler& operator= (const Scheduler&) = delete;

    void UpdateEvents(const uint64_t dt);

    void UpdateTriggers();

    void RegisterEvent(const Action& action, const uint64_t delay, const bool repeatable);

    void RegisterTrigger(const Action& action);

private:

    struct EventData
    {
        uint64_t mDelay;
        uint64_t mElapsedInterval;
        uint64_t mActionId;
        bool     mRepeatable;
        Action   mAction;
    };

    struct TriggerData
    {
        uint64_t mActionId;
        Action   mAction;
    };

    typedef std::list<EventData>   EventDataList;
    typedef std::list<TriggerData> TriggerDataList;
    typedef std::list<uint64_t>    UnregisteredActionList;

    void UnregisterEvent(const uint64_t actionId);

    void UnregisterTrigger(const uint64_t actionId);

    void CleanupEvents();

    void CleanupTriggers();

    EventDataList           mEvents;
    TriggerDataList         mTriggers;
    UnregisteredActionList  mUnregisteredEvents;
    UnregisteredActionList  mUnregisteredTriggers;
};

} // Pacman namespace