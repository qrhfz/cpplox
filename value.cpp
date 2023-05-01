#include "value.h"

#include <iostream>

namespace lox {
void initValueArray(ValueArray &arr) { arr.clear(); }
void printValue(Value value) { std::printf("%g", value); }

} // namespace lox
