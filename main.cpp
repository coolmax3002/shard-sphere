#include "server.hpp"
#include "RequestHandler.hpp"
#include <iostream>

int main (int argc, char *argv[]) {
  RequestHandler handler;
  Server server (8085, handler);
  if (server.start() == -1) {
    std::cout << "Failed to start server" << std::endl;
    return 1;
  }
  server.run();
  return 0;
}
