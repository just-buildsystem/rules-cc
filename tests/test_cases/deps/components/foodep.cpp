#include "foodep.hpp"

#include <iostream>
#include <ostream>

foo_t foodep(foo_t x) {
  std::cout << "foodep(" << x << ")" << std::endl;
  return x + 2;
}
