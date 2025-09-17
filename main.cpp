#include "server.h"
#include <iostream>

int main (int argc, char *argv[]) {
  Server server (8080);
  if (server.start() == -1) {
    std::cout << "Failed to start server" << std::endl;
    return 1;
  }
  server.run();
  return 0;
}
