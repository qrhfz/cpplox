#include "debug.h"

#include <iomanip>
#include <iostream>

#include "value.h"

namespace debug {

namespace {
void simpleInstruction(std::string name, size_t& offset) {
  std::cout << name << "\n";
  offset += 1;
}

void constantInstruction(std::string name, chunk::Chunk& chunk,
                         size_t& offset) {
  auto constantIdx = chunk.codes[offset + 1];
  std::printf("%-16s %4d '", name.c_str(), constantIdx);
  value::print(chunk.constants[constantIdx]);
  std::printf("'\n");
  offset += 2;
}

void disassembleInstruction(chunk::Chunk& chunk, size_t& offset) {
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
    case chunk::OP_RETURN:
      simpleInstruction("OP_RETURN", offset);
      break;

    default:
      std::cout << "Unknown opcode " << instruction << "\n";
      offset += 1;
      break;
  }
}
}  // namespace

void disassembleChunk(chunk::Chunk& chunk, std::string name) {
  std::cout << name << "\n";

  for (size_t offset = 0; offset < chunk.codes.size();) {
    disassembleInstruction(chunk, offset);
  }
}

}  // namespace debug
