#ifndef cpplox_value_h
#define cpplox_value_h

#include "object.h"
#include <algorithm>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace lox {

class Nil {};

enum class ValueType { Bool, Nil, Number, Object };

using Value = std::variant<double, bool, Nil, Object *>;
using ValueArray = std::vector<Value>;

struct TypeVisitor {
  ValueType operator()(double) { return ValueType::Number; }
  ValueType operator()(bool) { return ValueType::Bool; }
  ValueType operator()(Nil) { return ValueType::Nil; }
  ValueType operator()(Object *) { return ValueType::Object; }
};

inline ValueType getType(Value &value) {
  return std::visit(TypeVisitor{}, value);
}

void printValue(Value &);
} // namespace lox

#endif