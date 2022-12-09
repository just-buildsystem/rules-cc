#include "bar.hpp"
#include <iostream>

int bar(Foo *foo) {
  std::cout << "bar\n";
  return (foo == nullptr) ? -1 : foo->foo();
}
