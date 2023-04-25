#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include <cstdint>
#include <iostream>
#include <stack>

namespace vm {
InterpretResult VM::interpret(std::weak_ptr<chunk::Chunk> chunk) {
  this->chunk = chunk;

  if (!this->chunk.expired()) {
    this->ip = this->chunk.lock()->codes.data();
  }

  return run();
}

InterpretResult VM::run() {
  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    if (!this->chunk.expired()) {
      auto offset = this->ip - this->chunk.lock()->codes.data();

      debug::disassembleInstruction(*this->chunk.lock(), offset);
    }
#endif
    uint8_t instruction;
    switch (instruction = *this->ip++) {

    case chunk::OP_CONSTANT: {
      value::Value constant = readConstant();
      value::print(constant);
      std::cout << "\n";
      break;
    }
    case chunk::OP_RETURN:
      return INTERPRET_OK;
    }
  }
}

inline uint8_t VM::readByte() { return *this->ip++; }
inline value::Value VM::readConstant() {
  return this->chunk.lock()->constants.at(readByte());
}

void VM::init() { resetStack(); }
void VM::resetStack() { this->stack = std::stack<value::Value>(); }

} // namespace vm