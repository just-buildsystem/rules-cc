#include "foo/foo.hpp"
#include "baz.hpp"
#include <iostream>

int baz() {
  std::cout << "baz\n";
  return foo();
}
