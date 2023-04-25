#ifndef cpplox_debug_h
#define cpplox_debug_h

#include <string>

#include "chunk.h"

namespace debug {
void disassembleChunk(chunk::Chunk &chunk, std::string name);

int disassembleInstruction(chunk::Chunk &chunk, int offset);
} // namespace debug
#endif