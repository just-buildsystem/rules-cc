#include "bar/bar.hpp"
#include "foo/foo.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
  std::cout << foo() << " " << bar() << std::endl;
  return 0;
}
