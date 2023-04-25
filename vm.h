#ifndef cpplox_vm_h
#define cpplox_vm_h

#include <memory>

#include "chunk.h"
#include "value.h"
#include <stack>

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
  std::stack<value::Value> stack;

  InterpretResult run();
  inline uint8_t readByte();
  inline value::Value readConstant();
  void resetStack();

public:
  InterpretResult interpret(std::weak_ptr<chunk::Chunk> chunk);
  void init();
};

} // namespace vm

#endif