#pragma once

#include <unordered_map>
#include <string>

#include "error.h"
#include "utils.h"
#include "boost/any.hpp"

namespace Pacman {

class SharedDataContext
{
public:

    SharedDataContext() = default;
    SharedDataContext(const SharedDataContext&) = delete;
    ~SharedDataContext() = default;

    SharedDataContext& operator= (const SharedDataContext&) = delete;

    void SetValue(const std::string& name, const boost::any& value)
    {
        mValues[name] = value;
    }

    template <typename T>
    T GetValue(const std::string& name) const
    {
        auto iter = mValues.find(name);
        PACMAN_CHECK_ERROR2(iter != mValues.end(), MakeString("Unknown argument: ", name).c_str());
        return boost::any_cast<T>(iter->second);
    }

    bool HasValue(const std::string& name) const
    {
        return mValues.find(name) != mValues.end();
    }

    void Reset()
    {
        mValues.clear();
    }

private:

    std::unordered_map<std::string, boost::any> mValues;
};

} // Pacman namespace