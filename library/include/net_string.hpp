#pragma once
#include <cstddef>
#include <cstdlib>

#include <algorithm>
#include <stdexcept>
#include <string>

struct FromNetStringData {
};

struct FromPlainString {
};

class NetString {
public:
  NetString(FromNetStringData, const void* data, std::size_t bytes) : m_string{}
  {
    const char* const begin{static_cast<const char*>(data)};
    const char* const end{begin + bytes};
    const char* const colon{std::find(begin, end, ':')};

    if (colon == end) {
      throw std::runtime_error{"No colon in NetString"};
    }

    char*           strtollEnd{nullptr};
    const long long size{std::strtoll(begin, &strtollEnd, 10)};

    if (size <= 0) {
      throw std::runtime_error{"Couldn't parse size from NetString"};
    }

    if (strtollEnd != colon) {
      throw std::runtime_error{"NetString is malformatted"};
    }

    if ((colon + size) != (end - 1)) {
      throw std::runtime_error{"NetString is malformatted"};
    }

    if (*(end - 1) != ',') {
      throw std::runtime_error{"NetString doesn't end with a comma"};
    }

    m_string.assign(colon + 1, end - 1);
  }

  NetString(FromPlainString, const void* data, std::size_t bytes)
    : m_string(
      static_cast<const char*>(data),
      static_cast<const char*>(data) + bytes)
  {
  }

  std::string asNetString() const
  {
    std::string result{std::to_string(m_string.size())};
    result += ':';
    result += m_string;
    result += ',';
    return result;
  }

  std::string asPlainString() const
  {
    return m_string;
  }

private:
  std::string m_string;
};
