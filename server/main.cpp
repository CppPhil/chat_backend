#include <csignal>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

#include <fmt/format.h>

#include <Poco/Net/NetException.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/TCPServer.h>

#include "client_list_message.hpp"
#include "ip_address.hpp"
#include "net_string.hpp"
#include "ports.hpp"

namespace srv {
std::vector<Poco::Net::StreamSocket> gClients{};
std::mutex                           gClientsMutex{};

void disconnect()
{
  std::lock_guard<std::mutex> lock{gClientsMutex};

  for (Poco::Net::StreamSocket& socket : gClients) {
    socket.shutdown();
  }

  gClients.clear();
}

bool isAlive(const Poco::Net::StreamSocket& currentSocket)
{
  std::lock_guard<std::mutex> lock{gClientsMutex};
  const std::vector<Poco::Net::StreamSocket>::const_iterator it{
    std::find(gClients.begin(), gClients.end(), currentSocket)};
  return it != gClients.end();
}

void cleanClients()
{
  for (std::size_t i{0}; i < gClients.size(); ++i) {
    try {
      gClients[i].sendBytes("", 1);
    }
    catch (const Poco::Net::NetException& netException) {
      fmt::print(
        "Couldn't send null-terminator to client: \"{}\", removing client.\n",
        netException.what());
      gClients.erase(gClients.begin() + i);
      --i;
    }
  }
}

class ClientHandler : public Poco::Net::TCPServerConnection {
public:
  using TCPServerConnection::TCPServerConnection;

  void run() override
  {
    const Poco::Net::SocketAddress socketAddress{socket().peerAddress()};
    const std::string              address{socketAddress.toString()};

    {
      std::lock_guard<std::mutex> lock{gClientsMutex};
      cleanClients();
      gClients.push_back(socket());
      std::vector<std::string> ipAddresses(gClients.size(), std::string{});
      std::transform(
        gClients.begin(),
        gClients.end(),
        ipAddresses.begin(),
        [](const Poco::Net::StreamSocket& client) {
          return client.peerAddress().host().toString();
        });
      const lib::ClientListMessage clientListMessage{std::move(ipAddresses)};
      const std::string            json{clientListMessage.asJson()};
      const lib::NetString         netString{
        lib::FromPlainString{}, json.data(), json.size()};
      const std::string toSend{netString.asNetString()};

      for (Poco::Net::StreamSocket& client : gClients) {
        const int bytesToSend{static_cast<int>(toSend.size())};
        const int bytesSent{client.sendBytes(toSend.data(), bytesToSend)};

        if (bytesSent == bytesToSend) {
          fmt::print(
            "Sent \"{}\" to {}\n",
            toSend,
            client.peerAddress().host().toString());
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

    while (isAlive(socket())) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);
    }
  }
};

volatile std::sig_atomic_t gSignalState{0};

void signalHandler(int signal)
{
  gSignalState = signal;
}
} // namespace srv

int main()
{
  try {
    const Poco::Net::IPAddress     ipAddress{std::string{lib::ipAddress}};
    const Poco::Net::SocketAddress socketAddress{ipAddress, lib::tcpPort};
    const Poco::Net::ServerSocket  serverSocket{socketAddress};
    Poco::Net::TCPServer           tcpServer{
      Poco::Net::TCPServerConnectionFactory::Ptr{
        new Poco::Net::TCPServerConnectionFactoryImpl<srv::ClientHandler>{}},
      serverSocket};
    tcpServer.start();

    std::signal(SIGINT, &srv::signalHandler);
    fmt::print("Hit CTRL+C to exit.\n");

    while (srv::gSignalState != SIGINT) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);
    }

    fmt::print("Got CTRL+C, exiting.\n");
    srv::disconnect();
    tcpServer.stop();
  }
  catch (const Poco::Net::NetException& exception) {
    fmt::print(stderr, "Server caught NetException: {}\n", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
