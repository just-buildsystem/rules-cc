#include "bar.hpp"

#include "bardep.hpp"
#include <iostream>
#include <ostream>

int bar(int x) {
  std::cout << "bar(" << x << ")" << std::endl;
  return bardep(x) * 7;
}
