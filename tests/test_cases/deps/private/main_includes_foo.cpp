// test that foo.hpp not available

#include "foo/foo.hpp"
#include <iostream>

int main() {
  std::cout << "main\n";
  return 0;
}
