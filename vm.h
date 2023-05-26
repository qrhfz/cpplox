#ifndef cpplox_vm_h
#define cpplox_vm_h

#include <memory>

#include "chunk.h"
#include "value.h"
#include <deque>
#include <stack>
#include <string>

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

  InterpretResult run();
  inline uint8_t readByte();
  inline Value readConstant();
  void resetStack();

  template <typename Op> void binaryOp() {
    auto b = this->pop();
    auto a = this->pop();
    this->push(Op()(a, b));
  }

public:
  InterpretResult interpret(std::string const &src);
  void init();
  void push(Value);
  Value pop();
};

} // namespace lox

#endif