#include <cstdlib>
#include <csignal>

#include <iostream>
#include <vector>
#include <mutex>
#include <string>

#include <Poco/Net/NetException.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/StreamSocket.h>

#include "ip_address.hpp"
#include "ports.hpp"

std::vector<Poco::Net::StreamSocket> gClients{};
std::mutex gClientsMutex{};

class ClientHandler 
  : public Poco::Net::TCPServerConnection
{
public:
  using TCPServerConnection::TCPServerConnection;

  void run() override
  {
    for (;;) {
      const Poco::Net::SocketAddress socketAddress{socket().peerAddress()};
      const std::string address{socketAddress.toString()};
      std::lock_guard<std::mutex> lock{gClientsMutex};

      for (Poco::Net::StreamSocket& client : gClients) {
        // TODO: Send the ClientListMessage
      }
    
      gClients.push_back(socket());
    }
  }
};

volatile std::sig_atomic_t gSignalState{0};

void signalHandler(int signal)
{
  gSignalState = signal;
}

int main()
{
  try {
    const Poco::Net::IPAddress     ipAddress{ipAddress};
    const Poco::Net::SocketAddress socketAddress{ipAddress, tcpPort};
    const Poco::Net::ServerSocket  serverSocket{socketAddress};
    Poco::Net::TCPServer           tcpServer{
      Poco::Net::TCPServerConnectionFactory::Ptr{
        new Poco::Net::TCPServerConnectionFactoryImpl<ClientHandler>{}},
      serverSocket};
    tcpServer.start();

    std::signal(SIGINT, &signalHandler);
    std::cout << "Hit CTRL+C to exit.\n";

    while (gSignalState != SIGINT) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(100ms);
    }

    tcpServer.stop();
  } catch (const Poco::Net::NetException& exception) {
    std::cerr << "Server caught NetException: " << exception.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
