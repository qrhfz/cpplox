#ifndef cpplox_object_h
#define cpplox_object_h

#include "value.h"
#include <memory>
#include <string>
#include <variant>

namespace lox {
enum class ObjectType { String };

class Object {
public:
  ObjectType type;
};
using UniqueObjectPtr = std::unique_ptr<Object>;

class StringObject : public Object {
public:
  std::string str;

  StringObject(std::string str) : Object{ObjectType::String} {
    this->str = std::move(str);
  }
};

inline bool isObjectType(Value value, ObjectType type) {
  return isObject(value) && std::get<Object *>(value)->type == type;
}

inline bool isString(Value value) {
  return isObjectType(value, ObjectType::String);
}

inline StringObject *asString(Value value) {
  return static_cast<StringObject *>(asObject(value));
}

} // namespace lox

#endif