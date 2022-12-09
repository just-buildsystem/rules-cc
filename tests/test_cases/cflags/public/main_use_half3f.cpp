#include "half3f/half3f.hpp"
#include <iostream>

#ifdef HALF_PRECISION_DOUBLE
#error should not be defined
#endif

int main() {
  std::cout << half3f() << std::endl;
  return 0;
}
