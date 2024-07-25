#include "bar.hpp"

#include <iostream>
#include <ostream>

int bar(int x) {
  std::cout << "bar(" << x << ")" << std::endl;
  return x * 5;
}
