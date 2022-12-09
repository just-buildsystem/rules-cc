// test that foo.hpp is available (despite unused here)

#include "foo/foo.hpp"
#include <iostream>

int main() {
  std::cout << "main\n";
  return 0;
}
