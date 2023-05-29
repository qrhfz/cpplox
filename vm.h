#ifndef cpplox_vm_h
#define cpplox_vm_h

#include <memory>

#include "chunk.h"
#include "value.h"
#include <deque>
#include <stack>
#include <stdexcept>
#include <string>
#include <variant>

namespace lox {

inline bool isNumber(Value value) {
  return std::get_if<double>(&value) != nullptr;
}

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

  InterpretResult run();
  inline uint8_t readByte();
  inline Value readConstant();
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

  Value peek(int distance);

public:
  InterpretResult interpret(std::string const &src);
  void init();
  void push(Value);
  Value pop();
};

} // namespace lox

#endif