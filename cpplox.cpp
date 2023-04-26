#include <iostream>
#include <memory>

#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv) {
  std::shared_ptr<chunk::Chunk> chunk{new chunk::Chunk};

  auto constantIdx = chunk->addConstant(1.2);
  chunk->write(chunk::OP_CONSTANT, 123);
  chunk->write(constantIdx, 123);
  chunk->write(chunk::OP_NEGATE, 123);

  chunk->write(chunk::OP_RETURN, 123);

  // debug::disassembleChunk(*chunk, "test chunk");

  vm::VM vm{};
  vm.init();
  vm.interpret(chunk);
  return 0;
}
