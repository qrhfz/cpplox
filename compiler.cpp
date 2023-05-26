#include "compiler.h"
#include "chunk.h"
#include "scanner.h"
#include "value.h"
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

namespace lox {
bool Parser::compile(std::string const &src, Chunk &chunk) {
  scanner = std::make_unique<Scanner>(Scanner{src});
  *compilingChunk = chunk;

  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");

  return !hadError;
}

void Parser::advance() {
  previous = current;
  for (;;) {
    current = scanner->scanToken();
    if (current.type != TOKEN_ERROR) {
      break;
    }

    errorAtCurrent(current.str);
  }
}

void Parser::consume(TokenType type, std::string const &message) {
  if (current.type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

void Parser::errorAtCurrent(std::string message) { errorAt(current, message); }

void Parser::error(std::string message) { errorAt(previous, message); }

void Parser::errorAt(Token const &token, std::string message) {
  if (panicMode) {
    return;
  }
  panicMode = true;
  fprintf(stderr, "[line %d] Error", token.line);

  if (token.type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token.type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%s'", token.str.c_str());
  }

  fprintf(stderr, ": %s\n", message.c_str());
  hadError = true;
}

void Parser::emitByte(uint8_t byte) {
  writeChunk(*currentChunk(), byte, previous.line);
}

void Parser::emitBytes(uint8_t a, uint8_t b) {
  emitByte(a);
  emitByte(b);
}

Chunk *Parser::currentChunk() { return compilingChunk; }

void Parser::writeChunk(Chunk &chunk, uint8_t byte, int line) {}

void Parser::expression() {}
void Parser::number() {
  double value = std::stod(previous.str);
  emitConstant(value);
}

void Parser::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8_t Parser::makeConstant(Value value) {
  int constant = currentChunk()->addConstant(value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return constant;
}
} // namespace lox