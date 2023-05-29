#ifndef cpplox_value_h
#define cpplox_value_h

#include <variant>
#include <vector>

namespace lox {

class LoxNil {};

using Value = std::variant<double, bool, LoxNil>;
using ValueArray = std::vector<Value>;

void printValue(Value);
} // namespace lox

#endif