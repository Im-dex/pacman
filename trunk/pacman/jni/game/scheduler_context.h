#pragma once

#include <unordered_map>
#include <string>

#include "boost/any.hpp"

namespace Pacman {

class SchedulerContext
{
public:

    SchedulerContext() = default;
    SchedulerContext(const SchedulerContext&) = delete;
    ~SchedulerContext() = default;

    SchedulerContext& operator= (const SchedulerContext&) = delete;

    void SetValue(const std::string& name, const boost::any& value)
    {
        mValues[name] = value;
    }

    boost::any GetValue(const std::string& name) const
    {
        auto iter = mValues.find(name);
        if (iter == mValues.end())
            return boost::any();
        return iter->second;
    }

    void Reset()
    {
        mValues.clear();
    }

private:

    std::unordered_map<std::string, boost::any> mValues;
};

} // Pacman namespace