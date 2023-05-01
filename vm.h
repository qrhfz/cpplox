#ifndef cpplox_vm_h
#define cpplox_vm_h

#include <memory>

#include "chunk.h"
#include "value.h"
#include <deque>
#include <stack>
#include <string>

namespace vm {

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
private:
  std::weak_ptr<chunk::Chunk> chunk;
  uint8_t *ip;
  std::deque<value::Value> stack;

  InterpretResult run();
  inline uint8_t readByte();
  inline value::Value readConstant();
  void resetStack();

  template <typename Op> void binaryOp() {
    auto b = this->pop();
    auto a = this->pop();
    this->push(Op()(a, b));
  }

public:
  InterpretResult interpret(const std::string &src);
  void init();
  void push(value::Value);
  value::Value pop();
};

} // namespace vm

#endif