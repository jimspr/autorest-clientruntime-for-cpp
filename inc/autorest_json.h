#pragma once
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <autorest_util.h>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>
#include <chrono>

namespace autorest::json
{
  using namespace autorest::util;
// this file has overloads for deserializing json into different types

// Generic deserialize template which calls static member
// This template will be specialized for lots of other cases below.
// This works better than specializing function templates.
template <typename T>
struct serializer
{
  static rapidjson::Value serialize(const T& t, rapidjson::Value::AllocatorType &allocator)
  {
    return t.serialize(allocator);
  }
  static T deserialize(const rapidjson::Value &value)
  {
    // Generated types will have a json_deserialize method on them
    return T::deserialize(value);
  }
};

//Generic function for getting value as an out param
template <typename T>
void json_deserialize(T& out, const rapidjson::Value &value)
{
  out = serializer<T>::deserialize(value);
}

//Generic function for getting value as an out param
template <typename T>
void json_deserialize(T& out, const rapidjson::Value *value)
{
  if (value != nullptr)
    out = serializer<T>::deserialize(*value);
}

// Generic function for returning value
template <typename T>
T json_deserialize(const rapidjson::Value &value)
{
  return serializer<T>::deserialize(value);
}

// Generic function for returning value
template <typename T>
rapidjson::Value json_serialize(const T& value, rapidjson::Value::AllocatorType &allocator)
{
  return serializer<T>::serialize(value, allocator);
}

#if 0
// Specializations for various existing types
template <>
struct serializer<std::tm>
{
  static rapidjson::Value serialize(const std::tm tm, rapidjson::Value::AllocatorType &allocator)
  {
    // TODO - need better formatting
    rapidjson::Value v;
    char buf[128];
    sprintf(buf, "%4d-%2d-%2dT%2d-%2d-%2d", tm.year, tm.month, tm.mday, tm.hour, tm.min, tm.sec);
    return rapidjson::Value(buf, allocator);
  }
  static std::tm deserialize(const rapidjson::Value &value)
  {
    std::tm out; //TODO - handle milliseconds and time zone properly
    std::get_time(&out, "%Y-%m-%dT%H-%M-%S", value.GetString());
    return out;
  }
};

template <>
struct serializer<std::chrono::duration<float>>
{
  static rapidjson::Value serialize(const std::chrono::duration<float> duration, rapidjson::Value::AllocatorType &allocator)
  {
    rapidjson::Value v;
    return v;
  }
  static std::chrono::duration<float> deserialize(const rapidjson::Value &value)
  {
    std::chrono::duration<float> out; //TODO
    return out;
  }
};
#endif

template <>
struct serializer<int>
{
  static rapidjson::Value serialize(int value, rapidjson::Value::AllocatorType &allocator)
  {
    return rapidjson::Value(value);
  }
  static int deserialize(const rapidjson::Value &value)
  {
    return value.Get<int>();
  }
};

template <>
struct serializer<int64_t>
{
  static rapidjson::Value serialize(int64_t value, rapidjson::Value::AllocatorType &allocator)
  {
    return rapidjson::Value(value);
  }
  static int64_t deserialize(const rapidjson::Value &value)
  {
    return value.Get<int64_t>();
  }
};

template <>
struct serializer<double>
{
  static rapidjson::Value serialize(double value, rapidjson::Value::AllocatorType &allocator)
  {
    return rapidjson::Value(value);
  }
  static double deserialize(const rapidjson::Value &value)
  {
    return value.Get<double>();
  }
};

template <>
struct serializer<bool>
{
  static rapidjson::Value serialize(bool value, rapidjson::Value::AllocatorType &allocator)
  {
    return rapidjson::Value(value);
  }
  static bool deserialize(const rapidjson::Value &value)
  {
    return value.Get<bool>();
  }
};

template <>
struct serializer<std::string>
{
  static rapidjson::Value serialize(const std::string& value, rapidjson::Value::AllocatorType &allocator)
  {
    return rapidjson::Value(value.c_str(), allocator);
  }
  static std::string deserialize(const rapidjson::Value &value)
  {
    return value.GetString();
  }
};

template <typename T>
struct serializer<std::optional<T>>
{
  static rapidjson::Value serialize(const std::optional<T>& value, rapidjson::Value::AllocatorType &allocator)
  {
    if (value.has_value())
      return serialize(value.value(), allocator);
    return rapidjson::Value();
  }
  static std::optional<T> deserialize(const rapidjson::Value &value)
  {
    return json_deserialize<T>(value);
  }
};

template <typename T>
struct serializer<std::vector<T>>
{
  static rapidjson::Value serialize(const std::vector<T> &value, rapidjson::Value::AllocatorType &allocator)
  {
    rapidjson::Value v(rapidjson::kArrayType);
    for (auto &t : value)
      v.PushBack(json_serialize(t, allocator), allocator);
    return v;
  }
  static std::vector<T> deserialize(const rapidjson::Value &value)
  {
    std::vector<T> vec;
    for (auto &iter : value.GetArray())
    {
      vec.push_back(json_deserialize<T>(iter));
    }
    return vec;
  }
};

template <typename K, typename V>
struct serializer<std::unordered_map<K,V>>
{
  static rapidjson::Value serialize(const std::unordered_map<K, V>& map, rapidjson::Value::AllocatorType &allocator)
  {
    rapidjson::Value obj(rapidjson::kObjectType);
    for (auto& item : map)
    {
      rapidjson::Value name = json_serialize<K>(item.first, allocator);
      rapidjson::Value value = json_serialize<V>(item.second, allocator);
      obj.AddMember(name, value, allocator);
    }
    return obj;
  }
  static std::unordered_map<K, V> deserialize(const rapidjson::Value &value)
  {
    std::unordered_map<K, V> map;
    for (rapidjson::Value::ConstMemberIterator iter = value.MemberBegin(); iter != value.MemberEnd(); ++iter)
    {
      map[json_deserialize<K>(iter->name)] = json_deserialize<V>(iter->value);
    }
    return map;
  }
};

template <>
struct serializer<guid_t>
{
  static rapidjson::Value serialize(const guid_t& value, rapidjson::Value::AllocatorType &allocator)
  {
    wchar_t bufferW[64];
    StringFromGUID2(*(GUID*)&value, bufferW, sizeof(bufferW));
    char buffer[64];
    WideCharToMultiByte(CP_UTF8, 0, bufferW, -1, buffer, sizeof(buffer), nullptr, nullptr);
    return rapidjson::Value(buffer, allocator);
  }
  static guid_t deserialize(const rapidjson::Value &value)
  {
    wchar_t bufferW[64];
    MultiByteToWideChar(CP_UTF8, 0, value.GetString(), -1, bufferW, sizeof(bufferW));
    guid_t guid;
    CLSIDFromString(bufferW, (GUID*)&guid);
    return guid;
  }
};

template <>
struct serializer<byte_array_t>
{
  static rapidjson::Value serialize(const byte_array_t& value, rapidjson::Value::AllocatorType &allocator)
  {
    return rapidjson::Value(value.contents.c_str(), allocator);
  }
  static byte_array_t deserialize(const rapidjson::Value &value)
  {
    return byte_array_t { value.GetString() };
  }
};

}
