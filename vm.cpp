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
#include <stdexcept>
#include <tuple>
#include <valarray>
#include <variant>

namespace lox {
bool isFalsey(Value value);

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

    auto start = this->chunk.codes.data();
    size_t offset = this->ip - start;

    disassembleInstruction(this->chunk, offset);

#endif

    uint8_t instruction;
    try {
      switch (instruction = *this->ip++) {

      case OP_CONSTANT: {
        Value constant = readConstant();
        this->push(constant);
        break;
      }
      case OP_NIL:
        push(LoxNil{});
        break;
      case OP_TRUE:
        push(true);
        break;
      case OP_FALSE:
        push(false);
        break;
      case OP_ADD:
        this->binaryOp<std::plus<double>>();
        break;
      case OP_SUBTRACT:
        this->binaryOp<std::minus<double>>();
        break;
      case OP_MULTIPLY:
        this->binaryOp<std::multiplies<double>>();
        break;
      case OP_DIVIDE:
        this->binaryOp<std::divides<double>>();
        break;
      case OP_NOT:
        push(isFalsey(pop()));
        break;
      case OP_NEGATE: {
        if (double const *value = std::get_if<double>(&this->stack.back())) {
          this->stack.back() = -*value;
        } else {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_RETURN:
        printValue(this->pop());
        std::cout << "\n";
        return INTERPRET_OK;
      }

    } catch (std::runtime_error e) {
      runtimeError(e.what());
      return INTERPRET_RUNTIME_ERROR;
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
void VM::runtimeError(std::string message) {
  std::cerr << message << "\n";

  size_t instruction = ip - chunk.codes.data() - 1;
  int line = chunk.lines[instruction];
  std::cerr << "[line " << line << "] in script\n";
  resetStack();
}

Value VM::peek(int distance) { return stack.at(stack.size() - 1 - distance); }
bool isFalsey(Value value) {
  struct FalseyVisitor {
    bool operator()(bool b) { return !b; }
    bool operator()(double) { return false; }
    bool operator()(LoxNil) { return true; }
  };
  static FalseyVisitor visitor{};

  return std::visit(visitor, value);
}
} // namespace lox