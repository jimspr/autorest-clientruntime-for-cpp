#pragma once
#include <awaitcurl.h>
#pragma comment(lib, "ole32.lib")

namespace autorest::util
{
// String helpers
inline bool regex_is_match(const std::string& str, const std::string& regex)
{
//  __debugbreak();
  return true;
}

inline void replace_in_string(std::string& str, const std::string& olds, const std::string& news)
{
  size_t pos = 0u;
  while ((pos = str.find(olds, pos)) != std::string::npos)
  {
    str.replace(pos, olds.length(), news);
    pos += news.length();
  }
}

inline std::string string_format(const std::string& format, const std::string& str0)
{
  std::string out = format;
  replace_in_string(out, "{0}", str0);
  return out;
}

inline std::string string_join(const std::string& sep, const std::vector<std::string>& strs)
{
    std::string result;
    bool first = true;
    for (auto& str : strs)
    {
        if (first)
            first = false;
        else
            result += sep;
        result += str;
    }
    return result;
}

inline std::string escape_data_string(CURL* curl, const std::string& in)
{
  auto ptr = curl_easy_escape(curl, in.c_str(), in.size());
  std::string str = ptr;
  curl_free(ptr);
  return str;
}

struct cancellation_token_t
{
  void throw_if_cancellation_requested() {}
};

struct delegating_handler_t
{
};

template <typename T = std::monostate>
struct http_response : public ::awaitcurl::http_response_t
{
  http_response(const http_response_t& base) : http_response_t(base)
  {
  }
  http_response() = default;
  http_response(const http_response&) = default;
  http_response(http_response&&) = default;
  http_response& operator = (const http_response_t& base)
  {
    *this = http_response(base);
  }
  http_response& operator = (const http_response&) = default;
  http_response& operator = (http_response&&) = default;
  T object;
};

struct service_client_t
{
  std::vector<delegating_handler_t> handlers;
  ::awaitcurl::curl_requester_t& root_handler;
  // service_client_t(const std::vector<delegating_handler_t>& handlers) : handlers(handlers)
  // {
  // }
  service_client_t(::awaitcurl::curl_requester_t& root_handler, const std::vector<delegating_handler_t>& handlers) : root_handler(root_handler), handlers(handlers)
  {
  }
};

struct guid_t
{
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t  Data4[8];
};

struct service_client_credentials_t
{
};

struct validation_exception_t
{

};

template <typename T>
struct service_operations_t
{
  T& client;
  service_operations_t(T& t) : client(t)
  {

  }
};

// generic converter, call T::to_string
template <typename T>
struct string_converter
{
  static std::string to_string(const T& t)
  {
    return T::to_string(t);
  }
};

template <>
struct string_converter<std::string>
{
  static std::string to_string(const std::string& str)
  {
    return str;
  }
};

template <>
struct string_converter<int64_t>
{
  static std::string to_string(int64_t value)
  {
    return std::to_string(value);
  }
};

template <>
struct string_converter<int>
{
  static std::string to_string(int value)
  {
    return std::to_string(value);
  }
};

template <>
struct string_converter<long>
{
  static std::string to_string(long value)
  {
    return std::to_string(value);
  }
};

template <>
struct string_converter<bool>
{
  static std::string to_string(bool b)
  {
    return b ? "true" : "false";
  }
};

template <typename T>
inline std::string to_string(const T& t)
{
  return string_converter<T>::to_string(t);
}

}