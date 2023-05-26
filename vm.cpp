#include "vm.h"
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <valarray>

namespace lox {
InterpretResult VM::interpret(std::string const &src) {
  Parser parser{};
  Chunk chunk{};

  if (!parser.compile(src, chunk)) {
    return INTERPRET_COMPILE_ERROR;
  }

  this->chunk = std::move(chunk);
  ip = this->chunk.codes.data();

  return run();
}

InterpretResult VM::run() {
  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    std::printf("          ");
    for (auto slot : this->stack) {
      std::printf("[ ");
      printValue(slot);
      std::printf(" ]");
    }
    std::printf("\n");

    size_t offset = this->ip - this->chunk.codes.data();

    disassembleInstruction(this->chunk, offset);

#endif
    uint8_t instruction;
    switch (instruction = *this->ip++) {

    case OP_CONSTANT: {
      Value constant = readConstant();
      this->push(constant);
      break;
    }
    case OP_ADD:
      this->binaryOp<std::plus<Value>>();
      break;
    case OP_SUBTRACT:
      this->binaryOp<std::minus<Value>>();
      break;
    case OP_MULTIPLY:
      this->binaryOp<std::multiplies<Value>>();
      break;
    case OP_DIVIDE:
      this->binaryOp<std::divides<Value>>();
      break;
    case OP_NEGATE:
      this->stack.back() = -this->stack.back();
      break;
    case OP_RETURN:
      printValue(this->pop());
      std::cout << "\n";
      return INTERPRET_OK;
    }
  }
}

inline uint8_t VM::readByte() { return *this->ip++; }
inline Value VM::readConstant() { return this->chunk.constants.at(readByte()); }

void VM::push(Value value) { this->stack.push_back(value); }

Value VM::pop() {
  Value top{this->stack.back()};
  this->stack.pop_back();
  return top;
}

void VM::init() { resetStack(); }
void VM::resetStack() { this->stack.clear(); }

} // namespace lox