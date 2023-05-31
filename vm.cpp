#include "vm.h"
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
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

InterpretResult VM::interpret(std::string const &src) {
  Parser parser{*this};
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
    for (auto &slot : this->stack) {
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
        Value &constant = readConstant();
        this->push(std::move(constant));
        break;
      }
      case OP_NIL:
        push(Nil{});
        break;
      case OP_TRUE:
        push(true);
        break;
      case OP_FALSE:
        push(false);
        break;
      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();
        push(valuesEqual(std::move(a), std::move(b)));
        break;
      }
      case OP_GREATER:
        this->binaryOp<std::greater<double>>();
        break;
      case OP_LESS:
        this->binaryOp<std::less<double>>();
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
        Value v = this->pop();
        printValue(v);
        std::cout << "\n";
        return INTERPRET_OK;
      }

    } catch (std::runtime_error e) {
      runtimeError(e.what());
      return INTERPRET_RUNTIME_ERROR;
    }
  }
}

uint8_t VM::readByte() { return *this->ip++; }
Value &VM::readConstant() { return this->chunk.constants.at(readByte()); }

void VM::push(Value value) { this->stack.push_back(std::move(value)); }

Value VM::pop() {
  Value top = std::move(this->stack.back());
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

Value &VM::peek(int distance) { return stack.at(stack.size() - 1 - distance); }

bool isFalsey(Value value) {
  struct FalseyVisitor {
    bool operator()(bool b) { return !b; }
    bool operator()(double) { return false; }
    bool operator()(Nil) { return true; }
    bool operator()(Object *o) { return o != nullptr; }
  };
  static FalseyVisitor visitor{};

  return std::visit(visitor, value);
}

bool valuesEqual(Value a, Value b) {
  ValueType aType = getType(a);
  if (aType != getType(b)) {
    return false;
  }

  switch (aType) {
  case ValueType::Bool:
    return std::get<bool>(a) == std::get<bool>(b);
  case ValueType::Nil:
    return true;
  case ValueType::Number:
    return std::get<double>(a) == std::get<double>(b);
  case ValueType::Object:
    auto objA = std::get<Object *>(a);
    auto objB = std::get<Object *>(b);

    if (objA->type != objB->type) {
      return false;
    }

    if (objA->type == ObjectType::String) {
      return static_cast<StringObject *>(objA)->str ==
             static_cast<StringObject *>(objB)->str;
    }

    return objA == objB;
    break;
  }
}
} // namespace lox