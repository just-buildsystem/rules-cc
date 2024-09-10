#include "baz.hpp"

#include "qux.hpp"

void print_baz(std::string s) { print_qux("[baz]: " + s); }
