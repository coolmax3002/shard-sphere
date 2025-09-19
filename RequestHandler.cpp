#include "RequestHandler.hpp"
#include "HttpParser.hpp"
#include "json.hpp"
#include <iostream>
#include <optional>
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

  routes.push_back({
    std::regex("^/kvs/([^/]+)$"),
    {"GET", "PUT", "DELETE"},
    [this](const HttpRequest& request, const std::smatch matches) {
      return handleKvs(request, matches);
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

HttpResponse RequestHandler::handleKvs(const HttpRequest& request, const std::smatch matches) {
  const std::string key = matches[1];
  std::optional<nlohmann::json> value = kvs.get(key);

  if (request.method == "GET") {
    if (value.has_value()) {
      nlohmann::json response;
      response["result"] = "found";
      response["value"] = value;

      return HttpResponse("200", response.dump());
    } else {
      return HttpResponse("404", R"({"error":"Key does not exist"})");
    }
  } else if (request.method == "PUT") {
    try {
      nlohmann::json request_body = nlohmann::json::parse(request.body);

      if (!request_body.contains("value")) {
        return HttpResponse("400", R"({"error":"PUT request does not specify a value"})");
      } else if (key.length() > 50) {
        return HttpResponse("400", R"({"error":"Key is too long"})");
      }

      kvs.put(key, request_body["value"]);

      if (value.has_value()) {
        return HttpResponse("200", R"({"result":"replaced"})");
      } else {
        return HttpResponse("201", R"({"result":"created"})");
      }
    } catch (nlohmann::json::parse_error& e) {
      return HttpResponse("400", "Bad Request");
    }
  } else {
    //DELETE method
    if (value.has_value()) {
      kvs.deleteKey(key);
      return HttpResponse("200", R"({"result":"deleted"})");
    } else {
      return HttpResponse("404", R"({"error":"Key does not exist"})");
    }
  }
}
