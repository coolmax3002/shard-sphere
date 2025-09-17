#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <ostream>
#include <string>
#include <unordered_map>

struct HttpRequest {
  bool valid = false;
  std::string method;
  std::string path;
  std::string version;
  std::unordered_map<std::string, std::string> headers;
  std::string body;

  HttpRequest() = default;
  HttpRequest(bool valid) : valid(valid) {};
  HttpRequest(
      bool valid,
      std::string method,
      std::string path,
      std::string version,
      std::unordered_map<std::string, std::string> headers
      ) : valid(valid), method(method), path(path), version(version), headers(headers) {};

  friend std::ostream& operator<<(std::ostream& os, const HttpRequest& req) {
    os << "HttpRequest {\n";
    os << "  Valid: " << (req.valid ? "true" : "false") << "\n";
    os << "  Method: " << req.method << "\n";
    os << "  Path: " << req.path << "\n";
    os << "  Version: " << req.version << "\n";
    os << "  Headers: {\n";
    for (const auto& [key, value] : req.headers) {
      os << "    " << key << ": " << value << "\n";
    }
    os << "  }\n";
    os << "  Body: " << req.body << "\n";
    os << "}";
    return os;
  }
};

class HttpParser {
  private:
    std::string buffer;
    enum State { READING_HEADERS, READING_BODY, COMPLETE, FAILED };
    State state = READING_HEADERS;
    size_t header_end_pos = 0;
    size_t content_len = 0;

    HttpRequest validateRequest(const std::string& request);

  public: 
    enum PARSE_RESULT { NEED_MORE_DATA, PARSE_COMPLETE, PARSE_ERROR };
    HttpRequest request {false};

    PARSE_RESULT feed(const char* data, size_t len);
};

#endif // HTTP_PARSER_H
