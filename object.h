#ifndef cpplox_object_h
#define cpplox_object_h

#include "common.h"
#include <memory>
#include <string>
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
} // namespace lox

#endif