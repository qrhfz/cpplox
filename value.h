#ifndef cpplox_value_h
#define cpplox_value_h

#include <variant>
#include <vector>

namespace lox {
class Object;
class Nil {};

enum class ValueType {
  Bool,
  Nil,
  Number,
};

using Value = std::variant<double, bool, Nil>;
using ValueArray = std::vector<Value>;

struct TypeVisitor {
  ValueType operator()(double) { return ValueType::Number; }
  ValueType operator()(bool) { return ValueType::Bool; }
  ValueType operator()(Nil) { return ValueType::Nil; }
};

inline ValueType getType(Value value) {
  return std::visit(TypeVisitor{}, value);
}

void printValue(Value);
} // namespace lox

#endif