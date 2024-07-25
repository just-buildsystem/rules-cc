#include "foo.hpp"
#include "bar.hpp"

#include <iostream>
#include <ostream>

int main(int argc, char **argv) {
  std::cout << "Hello-from-main" << std::endl;
  std::cout << foo(bar(3)) << std::endl;
}
