#ifndef cpplox_value_h
#define cpplox_value_h

#include <algorithm>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace lox {

class Object;
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

void printValue(Value);

inline double asNumber(Value value) {
  return std::get<double>(value); //
}

inline bool asBool(Value value) {
  return std::get<bool>(value); //
}

inline Object *asObject(Value value) {
  return std::get<Object *>(value); //
}

inline bool isNumber(Value value) {
  return std::holds_alternative<double>(value);
}

inline bool isBool(Value value) {
  return std::holds_alternative<bool>(value); //
}

inline bool isNil(Value value) {
  return std::holds_alternative<Nil>(value); //
}

inline bool isObject(Value value) {
  return std::holds_alternative<Object *>(value); //
}

} // namespace lox

#endif