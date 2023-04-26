#ifndef cpplox_debug_h
#define cpplox_debug_h

#include <cstddef>
#include <string>

#include "chunk.h"

namespace debug {
void disassembleChunk(chunk::Chunk &chunk, std::string name);

void disassembleInstruction(chunk::Chunk &chunk, std::size_t &offset);
} // namespace debug
#endif