#include "foo/foo.hpp"
#include "bar.hpp"
#include <iostream>

int bar() {
  std::cout << "bar\n";
  return foo();
}
