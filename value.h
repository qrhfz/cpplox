#ifndef cpplox_value_h
#define cpplox_value_h

#include <vector>

namespace lox {
using Value = double;
using ValueArray = std::vector<Value>;

void initValueArray(ValueArray &);
void printValue(Value);
} // namespace lox

#endif