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
TEST(UrlTest, InvalidUrls) {
  EXPECT_THROW(Url("///www.foo.com"), utils::UrlError);
  EXPECT_THROW(Url("ftp://test.com"), utils::UrlError);
  EXPECT_THROW(Url("http:///"), utils::UrlError);
  EXPECT_THROW(Url("not a url"), utils::UrlError);
}

TEST(UrlTest, Resolve) {
  Url base("https://example.com/path/file.html");

  EXPECT_EQ(base.resolve("other.html").href(),
            "https://example.com/path/other.html");
  EXPECT_EQ(base.resolve("/root.html").href(), "https://example.com/root.html");
  EXPECT_EQ(base.resolve("http://other.site/").href(), "http://other.site/");
}

TEST(UrlTest, AboutScheme) {
  Url u("about:welcome");
  EXPECT_EQ(u.scheme(), "about");
  EXPECT_EQ(u.path(), "welcome");
}
