#pragma once

#include <cstdint>
#include <list>
#include <memory>

#include "trigger.h"

namespace Pacman {

class Scheduler
{
public:

    Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    ~Scheduler() = default;

    Scheduler& operator= (const Scheduler&) = delete;

    void Update(const uint64_t dt);

    void RegisterAction(const std::shared_ptr<Action> action, const uint64_t delay, const bool repeatable);

    void RegisterTrigger(const std::shared_ptr<Trigger> trigger);

private:

    struct ActionData
    {
        uint64_t                mDelay;
        uint64_t                mElapsedInterval;
        bool                    mRepeatable;
        std::shared_ptr<Action> mAction;
    };

    typedef std::list<ActionData> ActionDataList;
    typedef std::list<std::shared_ptr<Action>>  ActionList;
    typedef std::list<std::shared_ptr<Trigger>> TriggerList;

    void UnregisterAction(const std::shared_ptr<Action>& action);

    void UnregisterTrigger(const std::shared_ptr<Trigger>& trigger);

    void Cleanup();

    ActionDataList mActions;
    TriggerList    mTriggers;

    ActionList  mUnregisteredActions;
    TriggerList mUnregisteredTriggers;

    SchedulerContext mContext;
};

} // Pacman namespace