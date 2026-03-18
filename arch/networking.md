# Networking & Request Architecture

CSurfer uses a decoupled request system to handle various protocols and schemes.

## Interface: IRequest
The `IRequest` interface defines how the browser interacts with the network. As of Chapter 8, it supports sending data via payloads.

```cpp
virtual std::string request(const Url& url, const std::string& payload = "") = 0;
```

## HttpRequest Implementation
The `HttpRequest` class implements the HTTP/HTTPS protocol using Berkeley Sockets and OpenSSL.

### GET vs POST Logic
- If the `payload` is empty, it defaults to a `GET` request.
- If a `payload` is provided, it switches to `POST`, includes a `Content-Length` header, and appends the payload after the headers.

### Request Flow
1. **Socket Setup**: Creates a TCP socket.
2. **TLS Handshake**: If the scheme is `https`, performs an SSL handshake.
3. **HTTP Construction**: Builds the request string with appropriate headers.
4. **Transmission**: Sends the request over the socket/SSL.
5. **Collection**: Reads the response body after skipping the HTTP headers.
