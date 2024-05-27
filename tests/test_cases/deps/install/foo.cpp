#include "foo.hpp"
#include "baz/baz.hpp"
#include "qux.hpp"
#include <iostream>

int Foo::foo() {
  std::cout << "foo & inline " << baz_str() << std::endl;
  qux();
  return baz();
}
