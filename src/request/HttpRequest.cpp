#include "HttpRequest.h"
#include "url/Url.h"

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
HttpResponse HttpRequest::request(const Url &url, const std::string &payload) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return {};

  hostent *server = gethostbyname(url.host().c_str());
  if (!server) {
    close(sock);
    return {};
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(std::stoi(url.port()));
  std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sock);
    return {};
  }

  SSL_CTX *ctx = nullptr;
  SSL *ssl = nullptr;

  if (url.scheme() == "https") {
    SSL_library_init();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_client_method());
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    SSL_set_tlsext_host_name(ssl, url.host().c_str());

    if (SSL_connect(ssl) <= 0) {
      SSL_free(ssl);
      SSL_CTX_free(ctx);
      close(sock);
      return {};
    }
  }

  // Choose HTTP method based on payload presence.
  // If payload exists, use POST and include Content-Length.
  std::string method = payload.empty() ? "GET" : "POST";
  std::string req = method + " " + url.path() + " HTTP/1.0\r\n" +
                    "Host: " + url.host() + "\r\n";

  if (!payload.empty()) {
    req += "Content-Type: application/x-www-form-urlencoded\r\n";
    req += "Content-Length: " + std::to_string(payload.size()) + "\r\n";
  }

  req += "\r\n";

  if (!payload.empty()) {
    req += payload;
  }

  if (ssl) {
    SSL_write(ssl, req.c_str(), req.size());
  } else {
    send(sock, req.c_str(), req.size(), 0);
  }

  std::string response;
  char buffer[4096];

  while (true) {
    int bytes = ssl ? SSL_read(ssl, buffer, sizeof(buffer))
                    : read(sock, buffer, sizeof(buffer));
    if (bytes <= 0)
      break;
    response.append(buffer, bytes);
  }

  if (ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
  }

  close(sock);

  HttpResponse res;
  auto pos = response.find("\r\n\r\n");
  if (pos == std::string::npos) {
    res.body = response;
    return res;
  }

  std::string header_part = response.substr(0, pos);
  res.body = response.substr(pos + 4);

  std::istringstream stream(header_part);
  std::string line;
  if (std::getline(stream, line)) {
    // Skip status line (e.g., HTTP/1.0 200 OK)
  }

  while (std::getline(stream, line) && line != "\r") {
    auto colon = line.find(':');
    if (colon != std::string::npos) {
      std::string key = line.substr(0, colon);
      std::string value = line.substr(colon + 1);

      // Trim whitespace and carriage return
      auto trim = [](std::string &s) {
        s.erase(0, s.find_first_not_of(" \t"));
        auto end = s.find_last_not_of(" \r\t");
        if (end != std::string::npos)
          s.erase(end + 1);
        else
          s.clear();
      };

      trim(key);
      trim(value);

      // Normalize key to lowercase
      std::transform(key.begin(), key.end(), key.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      res.headers[key] = value;
    }
  }

  return res;
}
