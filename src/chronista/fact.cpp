#include "chronista/fact.hpp"

long int fact(const long int &number) {
  return number <= 1 ? number : fact(number - 1) * number;
}
