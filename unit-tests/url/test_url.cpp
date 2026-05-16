#include "request/IRequest.h"
#include "url/Url.h"
#include <gtest/gtest.h>

/**
 * Story: We test the URL class to ensure it correctly calculates the origin
 * and that HttpResponse stores headers correctly. This is critical for
 * security (Same-Origin Policy) and network requests.
 *
 * Why: URL parsing and origin calculation are security-critical.
 * What: Test origin calculation for various URL schemes and ports.
 * How: Using GoogleTest to assert expected origins against a list of test
 * cases.
 */

TEST(UrlTest, OriginCalculation) {
  struct TestCase {
    std::string url;
    std::string expected_origin;
  };

  std::vector<TestCase> test_cases = {
      {"https://google.com/search?q=test", "https://google.com:443"},
      {"http://example.com/", "http://example.com:80"},
      {"http://localhost:8080/path", "http://localhost:8080"},
      {"https://my-site.org:1234/test", "https://my-site.org:1234"},
      {"about:welcome", "about://:"}};

  for (const auto &test_case : test_cases) {
    Url url(test_case.url);
    EXPECT_EQ(url.origin(), test_case.expected_origin)
        << "Failed for URL: " << test_case.url;
  }
}

TEST(HttpResponseTest, HeaderStorage) {
  HttpResponse response;
  response.body = "test body";
  response.headers["content-type"] = "text/html";

  EXPECT_EQ(response.headers.size(), 1);
  EXPECT_EQ(response.headers["content-type"], "text/html");
}
