#include "RequestHandler.hpp"
#include "HttpParser.hpp"
#include "json.hpp"
#include <regex>
#include <cstddef>

void RequestHandler::registerRoutes() {
  routes.push_back({
    std::regex("^/ping$"),
    {"GET"},
    [this](const HttpRequest& request, const std::smatch matches) {
      return handlePing(request, matches);
    }
  });

  routes.push_back({
    std::regex("^/ping/([^/]+)$"),
    {"PUT"},
    [this](const HttpRequest& request, const std::smatch matches) {
      return handlePingName(request, matches);
    }
  });

  routes.push_back({
    std::regex("^/echo$"),
    {"GET", "PUT"},
    [this](const HttpRequest& request, const std::smatch matches) {
      return handleEcho(request, matches);
    }
  });
}

HttpResponse RequestHandler::processRequest(const HttpRequest& request) {
  for (const auto& route : routes) {
    std::smatch matches;
    if (std::regex_match(request.path, matches, route.pattern)) {
      if (route.allowed_methods.count(request.method) == 0) {
        return HttpResponse("405", "Method Not Allowed");
      }
      return route.handler(request, matches);
    }
  }
  return HttpResponse("404", "Not Found");
}


HttpResponse RequestHandler::handlePing(const HttpRequest& request, const std::smatch matches) {
  return HttpResponse("200", R"({"message":"I'm alive!!"})");
}

HttpResponse RequestHandler::handlePingName(const HttpRequest& request, const std::smatch matches) {
  nlohmann::json response = {{"message", "I'm alive, " + matches[1].str() + "!!"}};
  return HttpResponse("200", response.dump());
}

HttpResponse RequestHandler::handleEcho(const HttpRequest& request, const std::smatch matches) {
  if (request.method == "GET") {
    return HttpResponse("200", R"({"message":"Get Message Received"})");
  } else {
    try {
      nlohmann::json request_body = nlohmann::json::parse(request.body);
      if (!request_body.contains("message")) {
        return HttpResponse("400", "Bad Request");
      }

      nlohmann::json response = {{"message", request_body["message"]}};
      return HttpResponse("200", response.dump());
    } catch (nlohmann::json::parse_error& e) {
      return HttpResponse("400", "Bad Request");
    }
  }
}
