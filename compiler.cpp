#include "compiler.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "value.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

namespace lox {

bool Parser::compile(std::string const &src, Chunk &chunk) {
  scanner = std::make_unique<Scanner>(Scanner{src});
  compilingChunk = &chunk;

  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }
  endCompiler();

  return !hadError;
}

void Parser::synchronize() {
  panicMode = false;
  while (current.type != TOKEN_EOF) {
    if (previous.type == TOKEN_SEMICOLON) {
      return;
    }

    switch (current.type) {
    case TOKEN_CLASS:
    case TOKEN_FUN:
    case TOKEN_VAR:
    case TOKEN_FOR:
    case TOKEN_IF:
    case TOKEN_WHILE:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
      return;
    default:
      break;
    }

    advance();
  }
}

void Parser::declaration() {
  if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    statement();
  }

  if (panicMode) {
    synchronize();
  }
}

void Parser::varDeclaration() {
  uint8_t global = parseVariable("Expect variable name.");

  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }

  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  defineVariable(global);
}

uint8_t Parser::parseVariable(std::string errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  return identifierConstant(previous);
}

uint8_t Parser::identifierConstant(Token &name) {

  return makeConstant(addString(name.str));
}

void Parser::defineVariable(uint8_t global) {
  emitBytes(OP_DEFINE_GLOBAL, global);
}

void Parser::statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else {
    expressionStatement();
  }
}

void Parser::expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_POP);
}

void Parser::printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
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

bool Parser::match(TokenType type) {
  if (!check(type)) {
    return false;
  }

  advance();
  return true;
}

bool Parser::check(TokenType type) { return current.type == type; }

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

void Parser::expression() { parsePrecedence(Precedence::assignment); }
void Parser::number() {
  double value = std::stod(previous.str);
  emitConstant(value);
}

void Parser::string() {
  auto str = previous.str.substr(1, previous.str.length() - 2);

  auto ptr = addString(str);

  emitConstant(ptr);
}

Object *Parser::addString(std::string str) {
  return vm.addObject(std::make_unique<StringObject>(str));
}

void Parser::emitConstant(Value value) {
  emitBytes(OP_CONSTANT, makeConstant(std::move(value)));
}

uint8_t Parser::makeConstant(Value value) {
  int constant = currentChunk().addConstant(std::move(value));
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
  parsePrecedence(Precedence::unary);

  switch (operatorType) {
  case TOKEN_BANG:
    emitByte(OP_NOT);
    break;
  case TOKEN_MINUS:
    emitByte(OP_NEGATE);
    break;
  default:
    return;
  }
}

void Parser::binary() {
  TokenType operatorType = previous.type;
  ParseRule rule = getRule(operatorType);
  parsePrecedence(nextEnum(rule.precedence));

  switch (operatorType) {
  case TOKEN_BANG_EQUAL:
    emitBytes(OP_EQUAL, OP_NOT);
    break;
  case TOKEN_EQUAL_EQUAL:
    emitByte(OP_EQUAL);
    break;
  case TOKEN_GREATER:
    emitByte(OP_GREATER);
    break;
  case TOKEN_GREATER_EQUAL:
    emitBytes(OP_LESS, OP_NOT);
    break;
  case TOKEN_LESS:
    emitByte(OP_LESS);
    break;
  case TOKEN_LESS_EQUAL:
    emitBytes(OP_GREATER, OP_NOT);
    break;
  case TOKEN_PLUS:
    emitByte(OP_ADD);
    break;
  case TOKEN_MINUS:
    emitByte(OP_SUBTRACT);
    break;
  case TOKEN_STAR:
    emitByte(OP_MULTIPLY);
    break;
  case TOKEN_SLASH:
    emitByte(OP_DIVIDE);
    break;
  default:
    return;
  }
}

void Parser::literal() {
  switch (previous.type) {
  case TOKEN_FALSE:
    emitByte(OP_FALSE);
  case TOKEN_NIL:
    emitByte(OP_NIL);
  case TOKEN_TRUE:
    emitByte(OP_TRUE);
  default:
    return;
  }
}

ParseRule Parser::getRule(TokenType type) {
  switch (type) {
  case TOKEN_LEFT_PAREN:
    return {&Parser::grouping, nullptr, Precedence::none};
  case TOKEN_RIGHT_PAREN:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_LEFT_BRACE:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_RIGHT_BRACE:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_COMMA:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_DOT:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_MINUS:
    return {&Parser::unary, &Parser::binary, Precedence::term};
  case TOKEN_PLUS:
    return {nullptr, &Parser::binary, Precedence::term};
  case TOKEN_SEMICOLON:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_SLASH:
    return {nullptr, &Parser::binary, Precedence::factor};
  case TOKEN_STAR:
    return {nullptr, &Parser::binary, Precedence::factor};
  case TOKEN_BANG:
    return {&Parser::unary, nullptr, Precedence::none};
  case TOKEN_BANG_EQUAL:
    return {nullptr, &Parser::binary, Precedence::equality};
  case TOKEN_EQUAL:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_EQUAL_EQUAL:
    return {nullptr, &Parser::binary, Precedence::comparison};
  case TOKEN_GREATER:
    return {nullptr, &Parser::binary, Precedence::comparison};
  case TOKEN_GREATER_EQUAL:
    return {nullptr, &Parser::binary, Precedence::comparison};
  case TOKEN_LESS:
    return {nullptr, &Parser::binary, Precedence::comparison};
  case TOKEN_LESS_EQUAL:
    return {nullptr, &Parser::binary, Precedence::comparison};
  case TOKEN_IDENTIFIER:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_STRING:
    return {&Parser::string, nullptr, Precedence::none};
  case TOKEN_NUMBER:
    return {&Parser::number, nullptr, Precedence::none};
  case TOKEN_AND:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_CLASS:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_ELSE:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_FALSE:
    return {&Parser::literal, nullptr, Precedence::none};
  case TOKEN_FOR:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_FUN:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_IF:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_NIL:
    return {&Parser::literal, nullptr, Precedence::none};
  case TOKEN_OR:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_PRINT:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_RETURN:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_SUPER:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_THIS:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_TRUE:
    return {&Parser::literal, nullptr, Precedence::none};
  case TOKEN_VAR:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_WHILE:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_ERROR:
    return {nullptr, nullptr, Precedence::none};
  case TOKEN_EOF:
    return {nullptr, nullptr, Precedence::none};
    break;
  }
}

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