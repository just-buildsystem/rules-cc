#include "bar.hpp"
#include <iostream>

int bar(Foo *foo) {
  std::cout << "bar" << std::endl;
  return (foo == nullptr) ? -1 : foo->foo();
}
