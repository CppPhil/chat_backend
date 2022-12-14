#pragma once
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>

namespace lib {
class ClientListMessage {
public:
  ClientListMessage(std::vector<std::string> ipAddresses)
    : m_ipAddresses{std::move(ipAddresses)}
  {
  }

  /*!
   * \brief Creates a `ClientListMessage` from JSON.
   * \param json The JSON to parse as a `ClientListMessage`.
   * \throws std::runtime_error if parsing fails.
   **/
  ClientListMessage(const std::string& json) : m_ipAddresses{}
  {
    try {
      Poco::JSON::Parser           parser{};
      const Poco::Dynamic::Var     result{parser.parse(json)};
      const Poco::JSON::Array::Ptr arr{
        result.extract<Poco::JSON::Array::Ptr>()};

      for (const Poco::Dynamic::Var& ipAddress : *arr) {
        m_ipAddresses.push_back(ipAddress.toString());
      }
    }
    catch (const Poco::BadCastException& badCastException) {
      throw std::runtime_error{badCastException.what()};
    }
    catch (const Poco::InvalidAccessException& invalidAccessException) {
      throw std::runtime_error{invalidAccessException.what()};
    }
  }

  std::string asJson() const
  {
    Poco::JSON::Array array{};

    for (const std::string& ipAddress : m_ipAddresses) {
      array.add(Poco::Dynamic::Var{ipAddress});
    }

    std::ostringstream oss{};
    oss.imbue(std::locale::classic());
    array.stringify(oss);
    return oss.str();
  }

  const std::vector<std::string>& ipAddresses() const noexcept
  {
    return m_ipAddresses;
  }

private:
  std::vector<std::string> m_ipAddresses;
};
} // namespace lib
