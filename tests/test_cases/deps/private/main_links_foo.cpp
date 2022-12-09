// test that foo is linked

#include <iostream>

int foo(); // forward declare

int main() {
  std::cout << "main\n";
  return foo();
}
