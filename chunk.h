#ifndef clox_chunk_h
#define clox_chunk_h

#include <vector>

#include "common.h"
#include "value.h"

namespace lox {
enum OpCode {
  OP_CONSTANT,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_RETURN,
};

// using Chunk = std::vector<uint8_t>;

class Chunk {
public:
  std::vector<uint8_t> codes;
  std::vector<size_t> lines;
  ValueArray constants;

  void write(uint8_t byte, size_t line);
  void init();
  size_t getLine(size_t instructionIdx);

  uint64_t addConstant(Value);
};

} // namespace lox

#endif