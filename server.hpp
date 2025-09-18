#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include "RequestHandler.hpp"

class Server {
  private:
    int port;
    int server_fd;
    struct sockaddr_in address;
    RequestHandler handler;

    void sendBadRequest(int incoming_socket);
    void sendInternalErrorRequest(int incoming_socket);

  public:
    Server(int port, RequestHandler handler);
    ~Server();
    int start();
    void run();
    void stop();
};

#endif //SERVER_H
