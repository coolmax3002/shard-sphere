#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

class Server {
  private:
    int port;
    int server_fd;
    struct sockaddr_in address;

    void sendBadRequest(int incoming_socket);
    void sendInternalErrorRequest(int incoming_socket);

  public:
    Server(int port);
    ~Server();
    int start();
    void run();
    void stop();
};

#endif //SERVER_H
