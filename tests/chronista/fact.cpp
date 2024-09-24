#include "doctest/doctest.h"

TEST_CASE("dummy test") {
  int code = 1;
  const int &pain = code;
  CHECK(code == pain);
}
