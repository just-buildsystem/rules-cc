#include "foo.hpp"

#include "foodep.hpp"
#include <iostream>
#include <ostream>

int foo(int x) {
  std::cout << "foo(" << x << ")" << std::endl;
  return foodep(x) + 7;
}
