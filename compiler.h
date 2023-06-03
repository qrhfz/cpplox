#ifndef cpplox_compiler_h
#define cpplox_compiler_h

#include "chunk.h"
#include "scanner.h"
#include "vm.h"
#include <cstdint>
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
  Parser(VM &_vm) : vm(_vm) {}
  bool compile(std::string const &src, Chunk &chunk);

private:
  VM &vm;
  std::unique_ptr<Scanner> scanner = nullptr;
  Token current;
  Token previous;
  bool hadError = false;
  bool panicMode = false;
  Chunk *compilingChunk = nullptr;

  void synchronize();

  void declaration();

  void varDeclaration();
  uint8_t parseVariable(std::string);
  uint8_t identifierConstant(Token &);
  void defineVariable(uint8_t global);

  void statement();
  void printStatement();
  void expressionStatement();

  void expression();
  void number();
  void string();
  void grouping();
  void unary();
  void binary();
  void literal();
  void parsePrecedence(Precedence precedence);

  void advance();
  void consume(TokenType, std::string const &message);
  void endCompiler();
  bool match(TokenType);
  bool check(TokenType);

  void errorAtCurrent(std::string message);
  void error(std::string message);
  void errorAt(Token const &token, std::string message);

  Chunk &currentChunk();
  void emitByte(uint8_t byte);
  void emitBytes(uint8_t a, uint8_t b);
  void emitConstant(Value);
  void emitReturn();
  uint8_t makeConstant(Value value);
  Object *addString(std::string);
  ParseRule getRule(TokenType);
};

} // namespace lox

#endif