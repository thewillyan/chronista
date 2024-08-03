#include "chronista/fact.hpp"
#include <iostream>

int main() {
  const long int x{0};
  std::cout << x << "! = " << fact(x) << std::endl;
  return 0;
}
