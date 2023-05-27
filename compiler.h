#ifndef cpplox_compiler_h
#define cpplox_compiler_h

#include "chunk.h"
#include "scanner.h"
#include <memory>
#include <string>
#include <string_view>
namespace lox {
class Parser;
using ParseFn = void (lox::Parser::*)();

enum class Precedence {
  PREC_NONE,
  PREC_ASSIGNMENT, // =
  PREC_OR,         // or
  PREC_AND,        // and
  PREC_EQUALITY,   // == !=
  PREC_COMPARISON, // < > <= >=
  PREC_TERM,       // + -
  PREC_FACTOR,     // * /
  PREC_UNARY,      // ! -
  PREC_CALL,       // . ()
  PREC_PRIMARY
};

struct ParseRule {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
};

class Parser {
public:
  bool compile(std::string const &src, Chunk &chunk);

private:
  std::unique_ptr<Scanner> scanner;
  Token current;
  Token previous;
  bool hadError = false;
  bool panicMode = false;
  Chunk *compilingChunk;

  void expression();
  void number();
  void grouping();
  void unary();
  void binary();
  void parsePrecedence(Precedence precedence);

  void advance();
  void consume(TokenType, std::string const &message);
  void endCompiler();

  void errorAtCurrent(std::string message);
  void error(std::string message);
  void errorAt(Token const &token, std::string message);

  Chunk &currentChunk();
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t a, uint8_t b);
  void emitConstant(Value);
  void emitReturn();
  void writeChunk(Chunk &chunk, uint8_t byte, int line);
  uint8_t makeConstant(Value value);

  ParseRule &getRule(TokenType);

  ParseRule rules[40] = {
      {&Parser::grouping, NULL, Precedence::PREC_NONE}, // left paren
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {&Parser::unary, &Parser::binary, Precedence::PREC_TERM},
      {NULL, &Parser::binary, Precedence::PREC_TERM},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, &Parser::binary, Precedence::PREC_FACTOR},
      {NULL, &Parser::binary, Precedence::PREC_FACTOR},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {&Parser::number, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
      {NULL, NULL, Precedence::PREC_NONE},
  };
};

} // namespace lox

#endif