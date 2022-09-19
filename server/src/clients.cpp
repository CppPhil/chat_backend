#include <algorithm>

#include <fmt/format.h>

#include <Poco/Net/NetException.h>

#include "client_list_message.hpp"
#include "clients.hpp"
#include "net_string.hpp"

namespace srv {
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

  clean();
  m_sockets.push_back(socket);
  std::vector<std::string> ipAddresses(m_sockets.size(), std::string{});
  std::transform(
    m_sockets.begin(),
    m_sockets.end(),
    ipAddresses.begin(),
    [](const Poco::Net::StreamSocket& socket) {
      return socket.peerAddress().host().toString();
    });
  const lib::ClientListMessage clientListMessage{std::move(ipAddresses)};
  const std::string            json{clientListMessage.asJson()};
  const lib::NetString         netString{
    lib::FromPlainString{}, json.data(), json.size()};
  const std::string toSend{netString.asNetString()};

  for (Poco::Net::StreamSocket& socket : m_sockets) {
    const int bytesToSend{static_cast<int>(toSend.size())};
    const int bytesSent{socket.sendBytes(toSend.data(), bytesToSend)};

    if (bytesSent == bytesToSend) {
      fmt::print(
        "Sent \"{}\" to {}\n", toSend, socket.peerAddress().host().toString());
    }
    else {
      fmt::print(
        stderr,
        "Sent {} bytes, but should've sent {} bytes.\n",
        bytesSent,
        bytesToSend);
    }
  }
}

bool Clients::isAlive(const Poco::Net::StreamSocket& socket) const
{
  std::lock_guard<std::mutex> lock{m_mutex};
  return std::find(m_sockets.begin(), m_sockets.end(), socket)
         != m_sockets.end();
}

void Clients::clean()
{
  for (std::size_t i{0}; i < m_sockets.size(); ++i) {
    try {
      // Send 0x00 byte
      m_sockets[i].sendBytes("", 1);
    }
    catch (const Poco::Net::NetException& exception) {
      fmt::print(
        "Couldn't send null-terminator to client: \"{}\", removing client.\n",
        exception.what());
      m_sockets.erase(m_sockets.begin() + i);
      --i;
    }
  }
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
} // namespace srv
