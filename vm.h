#ifndef cpplox_vm_h
#define cpplox_vm_h

#include <algorithm>
#include <memory>

#include "chunk.h"
#include "object.h"
#include "value.h"
#include <deque>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace lox {

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
private:
  Chunk chunk;
  uint8_t *ip;
  std::deque<Value> stack;
  std::vector<UniqueObjectPtr> objects;
  std::unordered_map<StringObject *, Value> globals;

  InterpretResult run();
  uint8_t readByte();
  Value &readConstant();
  void resetStack();
  void runtimeError(std::string message);

  template <typename Op> void binaryOp() {
    if (!isNumber(peek(0)) || !isNumber(peek(1))) {
      throw std::runtime_error("Operand must be a number.");
    }

    auto b = std::get<double>(this->pop());
    auto a = std::get<double>(this->pop());
    push(Op()(a, b));
  }

  void concat();

  Value &peek(int distance);
  void push(Value);
  Value pop();

public:
  InterpretResult interpret(std::string const &src);
  void init();
  Object *addObject(UniqueObjectPtr ptr) {
    objects.push_back(std::move(ptr));
    return objects.back().get();
  }
};

bool isFalsey(Value value);
bool valuesEqual(Value a, Value b);

} // namespace lox

#endif