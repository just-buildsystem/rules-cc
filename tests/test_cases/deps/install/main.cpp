// test binary consuming libraries

#include "bar/bar.hpp"
#include <iostream>

int main() {
  std::cout << "main" << std::endl;
  Foo foo{};
  return bar(&foo);
}
