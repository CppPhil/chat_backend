#include <cstddef>

#include <fmt/format.h>

#include <Poco/Net/NetException.h>

#include <pl/algo/ranged_algorithms.hpp>

#include "lib/client_list_message.hpp"
#include "lib/contains.hpp"
#include "lib/net_string.hpp"

#include "clients.hpp"

namespace srv {
namespace {
std::string createNetString(std::vector<std::string>&& ipAddresses)
{
  const std::string json{
    lib::ClientListMessage{std::move(ipAddresses)}.asJson()};
  return lib::NetString{lib::FromPlainString{}, json.data(), json.size()}
    .asNetString();
}

std::string hostAddressOf(const Poco::Net::StreamSocket& socket)
{
  return socket.peerAddress().host().toString();
}
} // anonymous namespace

Clients::Clients() : m_mutex{}, m_padding{}, m_sockets{}
{
}

Clients::~Clients()
{
  disconnect();
}

void Clients::add(const Poco::Net::StreamSocket& socket)
{
  std::lock_guard<std::mutex> lock{m_mutex};

  m_sockets.push_back(socket);

  for (std::size_t i{0}; i < m_sockets.size(); ++i) {
    try {
      Poco::Net::StreamSocket& socket{m_sockets[i]};
      const std::string toSend{createNetString(createIpAddressesVector())};
      const int         bytesToSend{static_cast<int>(toSend.size())};
      const int         bytesSent{socket.sendBytes(toSend.data(), bytesToSend)};

      if (bytesSent == bytesToSend) {
        fmt::print("Sent \"{}\" to {}\n", toSend, hostAddressOf(socket));
      }
      else {
        fmt::print(
          stderr,
          "Sent {} bytes, but should've sent {} bytes.\n",
          bytesSent,
          bytesToSend);
      }
    }
    catch (const std::exception& exception) {
      m_sockets.erase(m_sockets.begin() + i);
      --i;
      fmt::print(
        "Erased client socket, couldn't send data to it: \"{}\".\n",
        exception.what());
    }
  }
}

bool Clients::isAlive(const Poco::Net::StreamSocket& socket) const
{
  std::lock_guard<std::mutex> lock{m_mutex};
  return lib::contains(m_sockets, socket);
}

void Clients::disconnect()
{
  std::lock_guard<std::mutex> lock{m_mutex};

  for (Poco::Net::StreamSocket& socket : m_sockets) {
    socket.shutdown();
  }

  // None of the sockets shall be 'alive'.
  m_sockets.clear();
}

std::vector<std::string> Clients::createIpAddressesVector()
{
  std::vector<std::string> ipAddresses(m_sockets.size(), std::string{});
  pl::algo::transform(m_sockets, ipAddresses.begin(), &hostAddressOf);
  return ipAddresses;
}
} // namespace srv
