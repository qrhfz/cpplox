#include "chunk.h"

namespace lox {

Chunk::Chunk() {}

Chunk::~Chunk() {}

// void Chunk::write(uint8_t byte, size_t line) { this->codes.push_back(byte); }

void Chunk::write(uint8_t byte, size_t line) {
  this->codes.push_back(byte);

  if (!this->lines.empty() && this->lines.back() == line) {
    this->lines.end()[-2] += 1;
  } else {
    this->lines.push_back(1);
    this->lines.push_back(line);
  }
}

void Chunk::init() { this->codes.clear(); }

size_t Chunk::getLine(size_t instructionIdx) {
  size_t length = 0;
  for (size_t i = 0; i < this->lines.size(); i += 2) {
    length += this->lines[i];

    if (length < instructionIdx) {
      continue;
    }

    return this->lines[i + 1];
  }

  return 0;
}

uint64_t Chunk::addConstant(Value value) {
  this->constants.push_back(value);

  return this->constants.size() - 1;
}

} // namespace lox
