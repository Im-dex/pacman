#pragma once

#include <cstdint>
#include <functional>
#include <list>
#include <memory>

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

    void Update(const uint64_t dt);

    void RegisterAction(const std::shared_ptr<Action>& action, const uint64_t delay, const bool repeatable);

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

    void UnregisterAction(const std::shared_ptr<Action>& action);

    void Cleanup();

    ActionDataList mActions;
    ActionList  mUnregisteredActions;
};

} // Pacman namespace