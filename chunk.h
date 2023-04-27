#ifndef clox_chunk_h
#define clox_chunk_h

#include <vector>

#include "common.h"
#include "value.h"

namespace chunk {
enum OpCode {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
};

// using Chunk = std::vector<uint8_t>;

class Chunk {
public:
  Chunk();
  ~Chunk();

  std::vector<uint8_t> codes;
  std::vector<size_t> lines;
  value::ValueArray constants;

  void write(uint8_t byte, size_t line);
  void init();
  size_t getLine(size_t instructionIdx);

  uint64_t addConstant(value::Value);
};

} // namespace chunk

#endif