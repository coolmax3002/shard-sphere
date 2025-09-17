#include "HttpParser.h"
#include <string>
#include <vector>
#include <sstream>

HttpParser::PARSE_RESULT HttpParser::feed(const char* data, size_t len) {
  buffer.append(data, len);

  if (state == READING_HEADERS) {
    size_t pos = buffer.find("\r\n\r\n");
    if (pos != std::string::npos) {
      header_end_pos = pos + 4;
      request = validateRequest(buffer.substr(0, pos));
      if (!request.valid){
        state = FAILED;
        return PARSE_ERROR;
      }

      if (request.headers.count("Content-Length")) {
        content_len = std::stoi(request.headers["Content-Length"]);
        state = READING_BODY;
      } else {
        state = COMPLETE;
        return PARSE_COMPLETE;
      }

    }
  } 
  if (state == READING_BODY) {
    if (buffer.length() >= header_end_pos + content_len) {
      request.body = buffer.substr(header_end_pos, content_len);
      state = COMPLETE;
      return PARSE_COMPLETE;
    }
  }

  return NEED_MORE_DATA;
}


HttpRequest HttpParser::validateRequest(const std::string& request) {
  const std::vector<std::string> HTTP_METHODS = {"GET", "PUT", "DELETE", "POST", "PATCH"};

  const int end_of_first_line = request.find("\r\n");
  const std::string first_request_line = request.substr(0, request.find("\r\n"));
  std::stringstream request_ss(first_request_line);
  std::string method, path, version;
  request_ss >> method >> path >> version;
  
  if (request_ss.fail() || !request_ss.eof() || method.empty() || path.empty() || version.empty()) {
    return HttpRequest(false);
  }

  auto it = std::find(HTTP_METHODS.begin(), HTTP_METHODS.end(), method);
  if (it == HTTP_METHODS.end()) {
    return HttpRequest(false);
  }

  if (path.length() < 1 || path[0] != '/' || version != "HTTP/1.1") {
    return HttpRequest(false);
  }

  const int start_of_headers = end_of_first_line + 2;
  const std::string all_headers = request.substr(start_of_headers);
  std::unordered_map<std::string, std::string> headers;

  std::stringstream header_ss(all_headers);
  std::string header;
  while (std::getline(header_ss, header)) {
    //TODO: write a trim function?
    if (header.empty() || header == "\r") {
      continue;
    }

    size_t colon_index = header.find(':');

    if (colon_index != std::string::npos) {
      std::string key = header.substr(0, colon_index);
      std::string value = header.substr(colon_index+2);
      if (key.empty() || value.empty()) {
        return HttpRequest(false);
      }
      headers[key] = value;
    } else {
      return HttpRequest(false);
    }

  }
  return HttpRequest(true, method, path, version, headers);
}
