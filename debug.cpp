#include "debug.h"

#include <cstddef>
#include <iomanip>
#include <iostream>

#include "chunk.h"
#include "value.h"

namespace lox {

namespace {
void simpleInstruction(std::string name, std::size_t &offset) {
  std::cout << name << "\n";
  offset += 1;
}

void constantInstruction(std::string name, Chunk &chunk, size_t &offset) {
  auto constantIdx = chunk.codes[offset + 1];
  std::printf("%-16s %4d '", name.c_str(), constantIdx);
  printValue(chunk.constants[constantIdx]);
  std::printf("'\n");
  offset += 2;
}

} // namespace

void disassembleChunk(Chunk &chunk, std::string name) {
  std::cout << name << "\n";

  for (size_t offset = 0; offset < chunk.codes.size();) {
    disassembleInstruction(chunk, offset);
  }
}

void disassembleInstruction(Chunk &chunk, size_t &offset) {
  std::cout << std::setw(4) << std::setfill('0') << offset << ' ';

  if (offset > 0 && chunk.getLine(offset) == chunk.getLine(offset - 1)) {
    std::printf("   | ");
  } else {
    printf("%4ld ", chunk.getLine(offset));
  }

  auto instruction = chunk.codes[offset];

  switch (instruction) {
  case OP_CONSTANT:
    constantInstruction("OP_CONSTANT", chunk, offset);
    break;
  case OP_NIL:
    return simpleInstruction("OP_NIL", offset);
  case OP_TRUE:
    return simpleInstruction("OP_TRUE", offset);
  case OP_FALSE:
    return simpleInstruction("OP_FALSE", offset);
  case OP_EQUAL:
    return simpleInstruction("OP_EQUAL", offset);
  case OP_GREATER:
    return simpleInstruction("OP_GREATER", offset);
  case OP_LESS:
    return simpleInstruction("OP_LESS", offset);
  case OP_ADD:
    return simpleInstruction("OP_ADD", offset);
  case OP_SUBTRACT:
    return simpleInstruction("OP_SUBTRACT", offset);
  case OP_MULTIPLY:
    return simpleInstruction("OP_MULTIPLY", offset);
  case OP_DIVIDE:
    return simpleInstruction("OP_DIVIDE", offset);
  case OP_NOT:
    return simpleInstruction("OP_NOT", offset);
  case OP_NEGATE:
    return simpleInstruction("OP_NEGATE", offset);
  case OP_PRINT:
    return simpleInstruction("OP_PRINT", offset);
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  default:
    std::cout << "Unknown opcode " << instruction << "\n";
    offset += 1;
    break;
  }
}

} // namespace lox
