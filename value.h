#ifndef cpplox_value_h
#define cpplox_value_h

#include <vector>

namespace value {
using Value = double;
using ValueArray = std::vector<Value>;

void initValueArray(ValueArray&);
void print(Value);
}  // namespace value

#endif