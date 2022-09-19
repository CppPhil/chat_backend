#include <csignal>
#include <cstdlib>

#include <chrono>
#include <memory>
#include <thread>

#include <gsl/util>

#include <fmt/format.h>

#include <Poco/Net/NetException.h>
#include <Poco/Net/TCPServer.h>

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

int main(int argc, char* argv[])
{
  constexpr int expectedArgc{2};
  constexpr int ipAddressIndex{1};

  if (argc != expectedArgc) {
    fmt::print(
      stderr,
      "Invalid command line argument count.\n"
      "Got {} command line arguments, but expected {}!\n"
      "\n"
      "Example:\n"
      "{} 192.168.178.24\n",
      argc,
      expectedArgc,
      argv[0]);
    return EXIT_FAILURE;
  }

  try {
    const Poco::Net::IPAddress     ipAddress{std::string{argv[ipAddressIndex]}};
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
