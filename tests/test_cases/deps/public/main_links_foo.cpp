// test that foo is linked

#include "foo/foo.hpp"
#include <iostream>

int main() {
  std::cout << "main\n";
  Foo{}.foo();
  return 0;
}
