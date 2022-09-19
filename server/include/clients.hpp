#pragma once
#include <cstddef>

#include <mutex>
#include <new>
#include <string>
#include <vector>

#include <Poco/Net/StreamSocket.h>

namespace srv {
class Clients {
public:
  Clients();

  Clients(const Clients&) = delete;
  Clients& operator=(const Clients&) = delete;

  ~Clients();

  void add(const Poco::Net::StreamSocket& socket);

  bool isAlive(const Poco::Net::StreamSocket& socket) const;

private:
  void clean();

  void disconnect();

  std::vector<std::string> createIpAddressesVector();

  mutable std::mutex m_mutex;
  unsigned char      m_padding[std::hardware_destructive_interference_size];
  std::vector<Poco::Net::StreamSocket> m_sockets;
};
} // namespace srv
