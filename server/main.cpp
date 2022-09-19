#include <csignal>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

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
    catch (...) {
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
          return client.peerAddress().toString();
        });
      const lib::ClientListMessage clientListMessage{std::move(ipAddresses)};
      const std::string            json{clientListMessage.asJson()};
      const lib::NetString         netString{
        lib::FromPlainString{}, json.data(), json.size()};
      const std::string toSend{netString.asNetString()};

      for (Poco::Net::StreamSocket& client : gClients) {
        const int bytesToSend{static_cast<int>(toSend.size())};
        const int bytesSent{client.sendBytes(toSend.data(), bytesToSend)};

        if (bytesSent != bytesToSend) {
          std::cerr << "Sent " << bytesSent << " bytes, but should've sent "
                    << bytesToSend << " bytes.\n";
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
    std::cout << "Hit CTRL+C to exit.\n";

    while (srv::gSignalState != SIGINT) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);
    }

    tcpServer.stop();
  }
  catch (const Poco::Net::NetException& exception) {
    std::cerr << "Server caught NetException: " << exception.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
