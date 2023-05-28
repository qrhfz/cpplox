#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "scanner.h"
#include "value.h"
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

namespace lox {

bool Parser::compile(std::string const &src, Chunk &chunk) {
  scanner = std::make_unique<Scanner>(Scanner{src});
  compilingChunk = &chunk;

  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");
  endCompiler();

  return !hadError;
}

void Parser::endCompiler() { emitReturn(); }

void Parser::emitReturn() {
  emitByte(OP_RETURN);
#ifdef DEBUG_PRINT_CODE
  if (!hadError) {
    disassembleChunk(currentChunk(), "code");
  }
#endif
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
  currentChunk().write(byte, previous.line);
}

void Parser::emitBytes(uint8_t a, uint8_t b) {
  emitByte(a);
  emitByte(b);
}

Chunk &Parser::currentChunk() { return *compilingChunk; }

// void Parser::writeChunk(Chunk &chunk, uint8_t byte, int line) {}

void Parser::expression() { parsePrecedence(Precedence::PREC_ASSIGNMENT); }
void Parser::number() {
  double value = std::stod(previous.str);
  emitConstant(value);
}

void Parser::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8_t Parser::makeConstant(Value value) {
  int constant = currentChunk().addConstant(value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return constant;
}

void Parser::grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Parser::unary() {
  TokenType operatorType = previous.type;

  // compile the operand
  parsePrecedence(Precedence::PREC_UNARY);

  switch (operatorType) {
  case TOKEN_MINUS:
    emitByte(OP_NEGATE);
    break;
  default:
    return;
  }
}

void Parser::binary() {
  TokenType operatorType = previous.type;
  ParseRule &rule = getRule(operatorType);
  parsePrecedence(nextEnum(rule.precedence));

  switch (operatorType) {
  case TOKEN_PLUS:
    emitByte(OP_ADD);
  case TOKEN_MINUS:
    emitByte(OP_SUBTRACT);
  case TOKEN_STAR:
    emitByte(OP_MULTIPLY);
  case TOKEN_SLASH:
    emitByte(OP_DIVIDE);
  default:
    return;
  }
}

ParseRule &Parser::getRule(TokenType type) { return rules[type]; }

void Parser::parsePrecedence(Precedence precedence) {
  advance();
  ParseFn prefixRule = getRule(previous.type).prefix;
  if (prefixRule == nullptr) {
    return error("Expect expression.");
  }

  (this->*prefixRule)();

  while (precedence <= getRule(current.type).precedence) {
    advance();
    ParseFn infixRule = getRule(previous.type).infix;
    (this->*infixRule)();
  }
}
} // namespace lox