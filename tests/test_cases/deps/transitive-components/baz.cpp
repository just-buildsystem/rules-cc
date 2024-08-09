#include "baz.hpp"

#include "bazdep.hpp"
#include <iostream>
#include <ostream>

int baz(int x) {
  std::cout << "baz(" << x << ")" << std::endl;
  return bazdep(x) * 13;
}
