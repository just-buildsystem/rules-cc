#include "plain.hpp"

bar_t foobar(foo_t x) {
  return bar(static_cast<bar_t>(foo(x)));
}
