#include "HttpRequest.h"
#include "url/Url.h"

#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <unistd.h>

// Berkeley sockets wrapper
std::string HttpRequest::request(const Url &url, const std::string &payload) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return "";

  hostent *server = gethostbyname(url.host().c_str());
  if (!server) {
    close(sock);
    return "";
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(std::stoi(url.port()));
  std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

  if (connect(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sock);
    return "";
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
      return "";
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

  auto pos = response.find("\r\n\r\n");
  return (pos != std::string::npos) ? response.substr(pos + 4) : response;
}
