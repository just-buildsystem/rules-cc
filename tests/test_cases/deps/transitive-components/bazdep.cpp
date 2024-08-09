#include "bazdep.hpp"

#include <iostream>
#include <ostream>

baz_t bazdep(baz_t x) {
  std::cout << "bazdep(" << x << ")" << std::endl;
  return x + 11;
}
