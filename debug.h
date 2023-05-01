#ifndef cpplox_debug_h
#define cpplox_debug_h

#include <cstddef>
#include <string>

#include "chunk.h"

namespace lox {
void disassembleChunk(Chunk &chunk, std::string name);

void disassembleInstruction(Chunk &chunk, std::size_t &offset);
} // namespace lox
#endif