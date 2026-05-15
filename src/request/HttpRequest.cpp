#include "HttpRequest.h"
#include "CookieJar.h"
#include "config/Config.h"
#include "url/Url.h"
#include "utils/StringUtils.h"

#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstring>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

// Berkeley sockets wrapper
HttpResponse HttpRequest::request(const Url &url, const std::string &payload,
                                  const Url &referrer) {
  int socket_handle = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_handle < 0)
    return {};

  hostent *server = gethostbyname(url.host().c_str());
  if (!server) {
    close(socket_handle);
    return {};
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(std::stoi(url.port()));
  std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(socket_handle, (sockaddr *)&addr, sizeof(addr)) < 0) {
    close(socket_handle);
    return {};
  }

  SSL_CTX *ctx = nullptr;
  SSL *ssl = nullptr;

  if (url.scheme() == "https") {
    SSL_library_init();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_client_method());
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socket_handle);
    SSL_set_tlsext_host_name(ssl, url.host().c_str());

    if (SSL_connect(ssl) <= 0) {
      SSL_free(ssl);
      SSL_CTX_free(ctx);
      close(socket_handle);
      return {};
    }
  }

  // Choose HTTP method based on payload presence.
  // If payload exists, use POST and include Content-Length.
  std::string http_method = payload.empty() ? "GET" : "POST";
  std::string request_string = http_method + " " + url.path() +
                               " HTTP/1.0\r\n" + "Host: " + url.host() + "\r\n";

  if (cookie_jar_) {
    std::string cookies = cookie_jar_->get_cookies(url, referrer, http_method);
    if (!cookies.empty()) {
      request_string += "Cookie: " + cookies + "\r\n";
    }
  }

  if (!referrer.host().empty()) {
    request_string += "Referer: " + referrer.origin() + "\r\n";
  }

  if (!payload.empty()) {
    request_string += "Content-Type: application/x-www-form-urlencoded\r\n";
    request_string +=
        "Content-Length: " + std::to_string(payload.size()) + "\r\n";
  }

  request_string += "\r\n";

  if (!payload.empty()) {
    request_string += payload;
  }

  if (ssl) {
    SSL_write(ssl, request_string.c_str(), request_string.size());
  } else {
    send(socket_handle, request_string.c_str(), request_string.size(), 0);
  }

  std::string response_text;
  char buffer[config::HTTP_BUFFER_SIZE];

  while (true) {
    int bytes_received = ssl ? SSL_read(ssl, buffer, sizeof(buffer))
                             : read(socket_handle, buffer, sizeof(buffer));
    if (bytes_received <= 0)
      break;
    response_text.append(buffer, bytes_received);
  }

  if (ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
  }

  close(socket_handle);

  HttpResponse res;
  auto pos = response_text.find("\r\n\r\n");
  if (pos == std::string::npos) {
    res.body = response_text;
    return res;
  }

  std::string header_part = response_text.substr(0, pos);
  res.body = response_text.substr(pos + 4);

  std::istringstream stream(header_part);
  std::string line;
  if (std::getline(stream, line)) {
    // Skip status line (e.g., HTTP/1.0 200 OK)
  }

  while (std::getline(stream, line) && line != "\r") {
    auto colon = line.find(':');
    if (colon != std::string::npos) {
      std::string key = utils::to_lower(utils::trim(line.substr(0, colon)));
      std::string value = utils::trim(line.substr(colon + 1));
      res.headers[key] = value;
    }
  }

  if (cookie_jar_ && res.headers.count("set-cookie")) {
    cookie_jar_->store_cookie(url, res.headers.at("set-cookie"));
  }

  return res;
}

std::string HttpRequest::get_cookies(const Url &url) {
  if (!cookie_jar_)
    return "";
  return cookie_jar_->get_cookies(url, url, "GET");
}

void HttpRequest::set_cookie(const Url &url, const std::string &value) {
  if (cookie_jar_) {
    cookie_jar_->store_cookie(url, value);
  }
}
