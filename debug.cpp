#include "debug.h"

#include <cstddef>
#include <iomanip>
#include <iostream>

#include "chunk.h"
#include "value.h"

namespace debug {

namespace {
void simpleInstruction(std::string name, std::size_t &offset) {
  std::cout << name << "\n";
  offset += 1;
}

void constantInstruction(std::string name, chunk::Chunk &chunk,
                         size_t &offset) {
  auto constantIdx = chunk.codes[offset + 1];
  std::printf("%-16s %4d '", name.c_str(), constantIdx);
  value::print(chunk.constants[constantIdx]);
  std::printf("'\n");
  offset += 2;
}

} // namespace

void disassembleChunk(chunk::Chunk &chunk, std::string name) {
  std::cout << name << "\n";

  for (size_t offset = 0; offset < chunk.codes.size();) {
    disassembleInstruction(chunk, offset);
  }
}

void disassembleInstruction(chunk::Chunk &chunk, size_t &offset) {
  std::cout << std::setw(4) << std::setfill('0') << offset << ' ';

  if (offset > 0 && chunk.getLine(offset) == chunk.getLine(offset - 1)) {
    std::printf("   | ");
  } else {
    printf("%4ld ", chunk.getLine(offset));
  }

  auto instruction = chunk.codes[offset];

  switch (instruction) {
  case chunk::OP_CONSTANT:
    constantInstruction("OP_CONSTANT", chunk, offset);
    break;
  case chunk::OP_NEGATE:
    return simpleInstruction("OP_NEGATE", offset);
  case chunk::OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  default:
    std::cout << "Unknown opcode " << instruction << "\n";
    offset += 1;
    break;
  }
}

} // namespace debug
