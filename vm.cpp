#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iostream>
#include <stack>
#include <valarray>

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
    std::printf("          ");
    for (auto slot : this->stack) {
      std::printf("[ ");
      value::print(slot);
      std::printf(" ]");
    }
    std::printf("\n");

    if (!this->chunk.expired()) {

      size_t offset = this->ip - this->chunk.lock()->codes.data();

      debug::disassembleInstruction(*this->chunk.lock(), offset);
    }
#endif
    uint8_t instruction;
    switch (instruction = *this->ip++) {

    case chunk::OP_CONSTANT: {
      value::Value constant = readConstant();
      // value::print(constant);
      // std::cout << "\n";
      this->push(constant);
      break;
    }
    case chunk::OP_ADD:
      this->binaryOp<std::plus<value::Value>>();
      break;
    case chunk::OP_SUBTRACT:
      this->binaryOp<std::minus<value::Value>>();
      break;
    case chunk::OP_MULTIPLY:
      this->binaryOp<std::multiplies<value::Value>>();
      break;
    case chunk::OP_DIVIDE:
      this->binaryOp<std::divides<value::Value>>();
      break;
    case chunk::OP_NEGATE:
      this->stack.back() = -this->stack.back();
      break;
    case chunk::OP_RETURN:
      value::print(this->pop());
      std::cout << "\n";
      return INTERPRET_OK;
    }
  }
}

inline uint8_t VM::readByte() { return *this->ip++; }
inline value::Value VM::readConstant() {
  return this->chunk.lock()->constants.at(readByte());
}

void VM::push(value::Value value) { this->stack.push_back(value); }

value::Value VM::pop() {
  value::Value top{this->stack.back()};
  this->stack.pop_back();
  return top;
}

void VM::init() { resetStack(); }
void VM::resetStack() { this->stack.clear(); }

} // namespace vm