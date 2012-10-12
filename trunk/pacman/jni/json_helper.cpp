#include "json_helper.h"

#include "error.h"

namespace Pacman {
namespace JsonHelper {

Value::Value(const std::string& data)
{
    Json::Reader reader;
    const bool parseResult = reader.parse(data, mRoot, false);
    PACMAN_CHECK_ERROR(parseResult && mRoot.isObject());
}

Value::Value(const Json::Value value)
     : mRoot(value)
{
}

template <>
std::string Value::GetAs<std::string>() const
{
    PACMAN_CHECK_ERROR(mRoot.isString());
    return mRoot.asString();
}

template <>
int8_t Value::GetAs<int8_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<int8_t>(mRoot.asInt());
}

template <>
int16_t Value::GetAs<int16_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<int16_t>(mRoot.asInt());
}

template <>
int32_t Value::GetAs<int32_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<int32_t>(mRoot.asInt());
}

template <>
int64_t Value::GetAs<int64_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<int64_t>(mRoot.asInt());
}

template <>
uint8_t Value::GetAs<uint8_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<uint8_t>(mRoot.asUInt());
}

template <>
uint16_t Value::GetAs<uint16_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<uint16_t>(mRoot.asUInt());
}

template <>
uint32_t Value::GetAs<uint32_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<uint32_t>(mRoot.asUInt());
}

template <>
uint64_t Value::GetAs<uint64_t>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<uint64_t>(mRoot.asUInt());
}

template <>
float Value::GetAs<float>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    return static_cast<float>(mRoot.asDouble());
}

template <>
double Value::GetAs<double>() const
{
    PACMAN_CHECK_ERROR(mRoot.isNumeric());
    mRoot.asDouble();
}

template <>
bool Value::GetAs<bool>() const
{
    PACMAN_CHECK_ERROR(mRoot.isBool());
    return mRoot.asBool();
}

template <>
Value Value::GetAs<Value>() const
{
    return *this;
}

template <>
Array Value::GetAs<Array>() const
{
    PACMAN_CHECK_ERROR(mRoot.isArray());
    return Array(mRoot);
}

template <>
Value Value::GetValue<Value>(const std::string& name) const
{
    const Json::Value val = mRoot[name];
    PACMAN_CHECK_ERROR(val.isObject());
    return Value(val);
}

//=========================================================================

ArrayIterator::ArrayIterator(const Json::Value value)
             : mValue(value),
               mPosition(0)
{
}

ArrayIterator::ArrayIterator(Json::Value value, const size_t position)
             : mValue(value),
              mPosition(position)
{
}

bool ArrayIterator::operator== (const ArrayIterator& other) const
{
    return mPosition == other.mPosition;
}

bool ArrayIterator::operator!= (const ArrayIterator& other) const
{
    return mPosition != other.mPosition;
}

Value ArrayIterator::operator* () const
{
    return Value(mValue[mPosition]);
}

ArrayIterator& ArrayIterator::operator++ ()
{
    ++mPosition;
    return *this;
}

ArrayIterator ArrayIterator::operator++ (int)
{
    return ArrayIterator(mValue, mPosition++);
}

Array::Array(const Json::Value value)
     : mValue(value)
{
}

Value Array::operator[] (const size_t index) const
{
    PACMAN_CHECK_ERROR(index < GetSize());
    return Value(mValue[index]);
}

size_t Array::GetSize() const
{
    return mValue.size();
}

ArrayIterator Array::begin() const
{
    return ArrayIterator(mValue);
}

ArrayIterator Array::end() const
{
    return ArrayIterator(mValue, GetSize());
}

} // JsonHelper namespace
} // Pacman namespace