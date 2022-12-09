#include "half3f/half3f.hpp"

#ifdef HALF_PRECISION_DOUBLE
#error should not be defined
#endif

int main() {
  auto result = half3f();
  return result > 1 and result < 2 ? 0 : 1;
}
