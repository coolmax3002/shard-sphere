#include "server.hpp"
#include "HttpParser.hpp"
#include "RequestHandler.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstddef>

Server::Server(int port, RequestHandler handler) : port(port), server_fd(-1), handler(handler) {
    memset(&address, 0, sizeof(address));
}

Server::~Server() {
    stop();
}

int Server::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        return -1;
    }
        
    std::cout << "Server listening on port: " << port << std::endl;
    return 0;
}

void Server::run() {
    if (server_fd < 0) {
        std::cerr << "Server has not been started." << std::endl;
        return;
    }

    socklen_t address_len = sizeof(address);

    while (1) {
        std::cout << "====================waiting for connection====================" << std::endl;

        int incoming_socket = accept(server_fd, (struct sockaddr*)&address, &address_len);
        if (incoming_socket < 0) {
            perror("incoming connection");
            continue;
        }

        HttpParser parser;
        char buffer[4096];
        ssize_t bytes_read = read(incoming_socket, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
          sendInternalErrorRequest(incoming_socket);
          continue;
        }

        auto result = parser.feed(buffer, bytes_read);

        int max_reads = 10;
        while (result == HttpParser::NEED_MORE_DATA && max_reads-- > 0) {
            bytes_read = read(incoming_socket, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
              sendInternalErrorRequest(incoming_socket);
              break;
            }
            result = parser.feed(buffer, bytes_read);
        }
        

        if (result == HttpParser::PARSE_COMPLETE) {
          HttpResponse response = handler.processRequest(parser.request);
          std::string response_string = response.toString();
          send(incoming_socket, response_string.c_str(), response_string.length(), 0);
          close(incoming_socket);
          std::cout << parser.request << std::endl;
          //handle request
        } else if (max_reads <= 0 || result == HttpParser::PARSE_ERROR) {
          sendBadRequest(incoming_socket);
        }
    }
}

void Server::sendBadRequest(int incoming_socket) {
  const char* response = 
    "HTTP/1.1 400 Bad Request\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 16\r\n"
    "\r\n"
    "Invalid Request!";
  send(incoming_socket, response, strlen(response), 0);
  close(incoming_socket);
}

void Server::sendInternalErrorRequest(int incoming_socket) {
  const char* response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
  send(incoming_socket, response, strlen(response), 0);
  close(incoming_socket);
}

void Server::stop() {
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
        std::cout << "server stopped" << std::endl;
    }
}
