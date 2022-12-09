#include "half3/half3.hpp"

#ifndef HALF_PRECISION_DOUBLE
#error should be defined
#endif

float half3f() { return static_cast<float>(half3()); }
