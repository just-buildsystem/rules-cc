#include "bardep.hpp"

#include <iostream>
#include <ostream>

bar_t bardep(bar_t x) {
  std::cout << "bardep(" << x << ")" << std::endl;
  return x + 5;
}
