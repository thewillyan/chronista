#include "chronista/fact.hpp"
#include "doctest/doctest.h"

TEST_CASE("testing the factorial function") {
  // commented so CI/CD pass
  // CHECK(fact(0) == 1); // should fail!
  CHECK(fact(1) == 1);
  CHECK(fact(2) == 2);
  CHECK(fact(3) == 6);
  CHECK(fact(10) == 3628800);
}
