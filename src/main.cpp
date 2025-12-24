#include <cassert>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

class URL {

private:
  std::string scheme;
  std::string host;
  std::string path;
  std::string port;

public:
  explicit URL(std::string url) {
    // Split scheme
    std::size_t scheme_pos = url.find("://");
    assert(scheme_pos != std::string::npos);

    scheme = url.substr(0, scheme_pos);
    assert(scheme == "http" || scheme == "https");

    // Remove scheme from url
    url = url.substr(scheme_pos + 3);

    // Ensure there is at least one '/'
    if (url.find('/') == std::string::npos) {
      url += '/';
    }

    // Split host and path
    std::size_t slash_pos = url.find('/');

    host = url.substr(0, slash_pos);

    if (auto colon = host.find(':'); colon != std::string::npos) {
      port = host.substr(colon + 1);
      host = host.substr(0, colon);
    }

    if (port.empty()) {
      port = (scheme == "https") ? "443" : "80";
    }

    path = url.substr(slash_pos); // already starts with '/'
  }

  std::string request(const std::string &host, const std::string &path,
                      const std::string &port) const {
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      perror("socket");
      return "";
    }

    // Resolve host
    struct hostent *server = gethostbyname(host.c_str());
    if (!server) {
      std::cerr << "No such host: " << host << std::endl;
      close(sock);
      return "";
    }

    // Setup server address
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(std::stoi(port));
    std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("connect");
      close(sock);
      return "";
    }

    SSL_CTX *ctx = nullptr;
    SSL *ssl = nullptr;

    if (scheme == "https") {
      SSL_library_init();
      SSL_load_error_strings();

      ctx = SSL_CTX_new(TLS_client_method());
      ssl = SSL_new(ctx);
      SSL_set_fd(ssl, sock);
      SSL_set_tlsext_host_name(ssl, host.c_str());

      if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return "";
      }
    }

    // Build HTTP GET request
    std::string request = "GET " + path +
                          " HTTP/1.0\r\n"
                          "Host: " +
                          host +
                          "\r\n"
                          "\r\n";

    // Send request
    if (scheme == "https") {
      SSL_write(ssl, request.c_str(), request.size());
    } else {
      send(sock, request.c_str(), request.size(), 0);
    }

    // Read response

    std::string response;
    char buffer[4096];
    int bytes = 0;

    while (true) {
      if (scheme == "https") {
        bytes = SSL_read(ssl, buffer, sizeof(buffer));
      } else {
        bytes = read(sock, buffer, sizeof(buffer));
      }

      if (bytes <= 0)
        break;

      response.append(buffer, bytes);
    }

    if (scheme == "https") {
      SSL_shutdown(ssl);
      SSL_free(ssl);
      SSL_CTX_free(ctx);
    }

    close(sock);

    // Optional: skip headers and return only body
    auto pos = response.find("\r\n\r\n");
    if (pos != std::string::npos)
      return response.substr(pos + 4);

    return response;
  }

  void load() {
    const std::string body =
        this->request(this->getHost(), this->getPath(), this->getPort());
    this->show(body);
  }

  void show(const std::string &body) {
    bool in_tag = false;

    // print text and skip tags
    for (const char c : body) {
      if (c == '<') {
        in_tag = true;
      } else if (c == '>') {
        in_tag = false;
      } else if (!in_tag) {
        std::cout << c;
      }
    }
  }

  std::string getScheme() const { return scheme; }
  std::string getHost() const { return host; }
  std::string getPath() const { return path; }
  std::string getPort() const { return port; }
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: browser <url>\n";
    return 1;
  }

  URL url(argv[1]);
  url.load();
}
