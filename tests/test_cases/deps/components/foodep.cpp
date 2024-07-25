#include "foodep.hpp"

#include <iostream>
#include <ostream>

int foodep(int x) {
  std::cout << "foodep(" << x << ")" << std::endl;
  return x + 2;
}
