#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "HttpParser.hpp"
#include "KeyValueStore.hpp"
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>
#include <cstddef>

struct Route {
  std::regex pattern;
  std::unordered_set<std::string> allowed_methods;
  std::function<HttpResponse(const HttpRequest&, const std::smatch& matches)> handler;
};

class RequestHandler {
  private:
    std::vector<Route> routes;
    KeyValueStore kvs;

  public:
    RequestHandler() : kvs() {
      registerRoutes();
    }

    RequestHandler(const KeyValueStore& existing_kvs) : kvs(existing_kvs) {
      registerRoutes();
    }

    void registerRoutes();
    HttpResponse processRequest(const HttpRequest& request);

  private:
    HttpResponse handlePing(const HttpRequest& request, const std::smatch matches);
    HttpResponse handlePingName(const HttpRequest& request, const std::smatch matches);
    HttpResponse handleEcho(const HttpRequest& request, const std::smatch matches);
    HttpResponse handleKvs(const HttpRequest& request, const std::smatch matches);
};

#endif //REQUEST_HANDLER_H 
