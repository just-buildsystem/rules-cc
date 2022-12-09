#include "half.hpp"

HALF_RESULT_TYPE half(int val) {
  return static_cast<HALF_RESULT_TYPE>(val / 2.0);
}
