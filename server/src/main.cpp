#include <csignal>
#include <cstdlib>

#include <chrono>
#include <memory>
#include <thread>

#include <gsl/util>

#include <fmt/format.h>

#include <Poco/Net/NetException.h>
#include <Poco/Net/TCPServer.h>

#include "lib/ip_address.hpp"
#include "lib/ports.hpp"

#include "clients.hpp"

namespace srv {
std::unique_ptr<Clients> gClients{std::make_unique<Clients>()};

class ClientHandler : public Poco::Net::TCPServerConnection {
public:
  using TCPServerConnection::TCPServerConnection;

  void run() override
  {
    Poco::Net::StreamSocket& streamSocket{socket()};
    gClients->add(streamSocket);

    while (gClients->isAlive(streamSocket)) {
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
    auto scopeGuard{gsl::finally([&tcpServer] {
      srv::gClients = nullptr;
      tcpServer.stop();
    })};

    while (srv::gSignalState != SIGINT) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);
    }

    fmt::print("Got CTRL+C, exiting.\n");
  }
  catch (const Poco::Net::NetException& exception) {
    fmt::print(stderr, "Server caught NetException: {}\n", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
