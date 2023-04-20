#include <iostream>

#include "chunk.h"
#include "debug.h"

int main(int argc, char **argv) {
  chunk::Chunk chunk;

  auto constantIdx = chunk.addConstant(1.2);
  chunk.write(chunk::OP_CONSTANT, 123);
  chunk.write(constantIdx, 123);

  chunk.write(chunk::OP_RETURN, 123);
  debug::disassembleChunk(chunk, "test chunk");
  return 0;
}
