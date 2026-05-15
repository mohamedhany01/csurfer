#include "request/HttpRequest.h"
#include <gtest/gtest.h>

TEST(HttpRequestTest, ParseResponseHeaders) {
  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: 12\r\n"
                         "Set-Cookie: session=123\r\n"
                         "\r\n"
                         "Hello World!";

  HttpResponse res = HttpRequest::parse_response(response);

  EXPECT_EQ(res.body, "Hello World!");
  EXPECT_EQ(res.headers.at("content-type"), "text/html");
  EXPECT_EQ(res.headers.at("content-length"), "12");
  EXPECT_EQ(res.headers.at("set-cookie"), "session=123");
}

TEST(HttpRequestTest, ParseMalformedResponse) {
  std::string response = "No headers here";
  HttpResponse res = HttpRequest::parse_response(response);

  EXPECT_EQ(res.body, "No headers here");
  EXPECT_TRUE(res.headers.empty());
}
