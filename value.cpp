#include "value.h"

#include <iostream>

namespace value {
void initValueArray(ValueArray& arr) { arr.clear(); }
void print(Value value) { std::printf("%g", value); }

}  // namespace value
