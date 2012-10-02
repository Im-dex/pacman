#pragma once

#include <string>

#include "base.h"
#include "json/json.h"

namespace Pacman {
namespace JsonHelper {

class Value
{
public:

    Value(const std::string& data);
    Value(const Json::Value value);
    Value(const Value&) = default;
    ~Value() = default;

    Value& operator= (const Value&) = default;

    template <typename T>
    T GetAs() const;

    template <typename T>
    T GetValue(const std::string& name) const
    {
        return Value(mRoot[name]).GetAs<T>();
    }

private:

    Json::Value mRoot;
};

//=========================================================================

class ArrayIterator
{
public:

    ArrayIterator(Json::Value value);
    ArrayIterator(Json::Value value, const size_t position);
    ArrayIterator(const ArrayIterator&) = default;
    ~ArrayIterator() = default;

    ArrayIterator& operator= (const ArrayIterator&) = default;

    bool operator== (const ArrayIterator& other) const;

    bool operator!= (const ArrayIterator& other) const;

    Value operator* () const;

    ArrayIterator& operator++ ();

    ArrayIterator operator++ (int);

private:

    size_t      mPosition;
    Json::Value mValue;
};

class Array
{
public:

    Array(const Json::Value value);
    Array(const Array&) = default;
    ~Array() = default;

    Array& operator= (const Array&) = default;

    Value operator[] (const size_t index) const;

    size_t GetSize() const;

    ArrayIterator begin() const;

    ArrayIterator end() const;

private:

    Json::Value mValue;
};

} // JsonHelper namespace
} // Pacman namespace