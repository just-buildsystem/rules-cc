// test that foo is linked after bar

#include "bar/bar.hpp"
#include <iostream>

int main() {
  std::cout << "main\n";
  return bar();
}
