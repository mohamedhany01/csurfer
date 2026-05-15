#include "request/IRequest.h"
#include "url/Url.h"
#include <gtest/gtest.h>

TEST(UrlTest, OriginCalculation) {
  struct TestCase {
    std::string url;
    std::string expected_origin;
  };

  std::vector<TestCase> cases = {
      {"https://google.com/search?q=test", "https://google.com:443"},
      {"http://example.com/", "http://example.com:80"},
      {"http://localhost:8080/path", "http://localhost:8080"},
      {"https://my-site.org:1234/test", "https://my-site.org:1234"},
      {"about:welcome", "about://:"}};

  for (const auto &tc : cases) {
    Url u(tc.url);
    EXPECT_EQ(u.origin(), tc.expected_origin) << "Failed for URL: " << tc.url;
  }
}

TEST(HttpResponseTest, HeaderStorage) {
  HttpResponse res;
  res.body = "test body";
  res.headers["content-type"] = "text/html";

  EXPECT_EQ(res.headers.size(), 1);
  EXPECT_EQ(res.headers["content-type"], "text/html");
}
