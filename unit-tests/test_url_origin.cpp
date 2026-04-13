#include "request/IRequest.h"
#include "url/Url.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct TestCase {
  std::string url;
  std::string expected_origin;
};

int main() {
  std::vector<TestCase> cases = {
      {"https://google.com/search?q=test", "https://google.com:443"},
      {"http://example.com/", "http://example.com:80"},
      {"http://localhost:8080/path", "http://localhost:8080"},
      {"https://my-site.org:1234/test", "https://my-site.org:1234"},
      {"about:welcome", "about://:"} // 'about' scheme doesn't have a host/port
  };

  int failed = 0;
  std::cout << "--- Running URL Origin Tests ---" << std::endl;

  for (const auto &tc : cases) {
    Url u(tc.url);
    std::string actual = u.origin();
    if (actual == tc.expected_origin) {
      std::cout << "SUCCESS: " << tc.url << " -> " << actual << std::endl;
    } else {
      std::cout << "FAILURE: " << tc.url
                << "\n  Expected: " << tc.expected_origin
                << "\n  Actual:   " << actual << std::endl;
      failed++;
    }
  }

  std::cout << "\n--- HttpResponse Structure Test ---" << std::endl;
  HttpResponse res;
  res.body = "test body";
  res.headers["content-type"] = "text/html";
  if (res.headers.size() == 1 && res.headers["content-type"] == "text/html") {
    std::cout << "SUCCESS: HttpResponse headers verified." << std::endl;
  } else {
    std::cout << "FAILURE: HttpResponse headers mismatch." << std::endl;
    failed++;
  }

  if (failed == 0) {
    std::cout << "\nALL TESTS PASSED!" << std::endl;
    return 0;
  } else {
    std::cout << "\n" << failed << " TEST(S) FAILED!" << std::endl;
    return 1;
  }
}
