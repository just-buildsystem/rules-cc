#include "foo.hpp"
#include "bar.hpp"
#include "baz.hpp"

#include <iostream>
#include <ostream>

int main(int argc, char **argv) {
  std::cout << "Hello-from-main" << std::endl;
  std::cout << foo(bar(baz(13))) << std::endl;
}
