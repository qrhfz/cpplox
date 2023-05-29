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
  none,
  assignment, // =
  OR,         // or
  AND,        // and
  equality,   // == !=
  comparison, // < > <= >=
  term,       // + -
  factor,     // * /
  unary,      // ! -
  call,       // . ()
  primary
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
  void literal();
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
  uint8_t makeConstant(Value value);

  ParseRule getRule(TokenType);
};

} // namespace lox

#endif