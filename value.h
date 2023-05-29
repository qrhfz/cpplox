#ifndef cpplox_value_h
#define cpplox_value_h

#include <variant>
#include <vector>

namespace lox {

class LoxNil {};

enum class ValueType {
  Bool,
  Nil,
  Number,
};

using Value = std::variant<double, bool, LoxNil>;
using ValueArray = std::vector<Value>;

inline ValueType getType(Value value) {
  struct TypeVisitor {
    ValueType operator()(double) { return ValueType::Number; }
    ValueType operator()(bool) { return ValueType::Bool; }
    ValueType operator()(LoxNil) { return ValueType::Nil; }
  };
  static TypeVisitor visitor{};
  return std::visit(visitor, value);
}

void printValue(Value);
} // namespace lox

#endif