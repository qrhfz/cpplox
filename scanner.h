#ifndef cpplox_scanner_h
#define cpplox_scanner_h
#include <string>
#include <string_view>

// using std::string;
using std::string_view;

namespace lox {

typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  // One or two character tokens.
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS,
  TOKEN_LESS_EQUAL,
  // Literals.
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_NUMBER,
  // Keywords.
  TOKEN_AND,
  TOKEN_CLASS,
  TOKEN_ELSE,
  TOKEN_FALSE,
  TOKEN_FOR,
  TOKEN_FUN,
  TOKEN_IF,
  TOKEN_NIL,
  TOKEN_OR,
  TOKEN_PRINT,
  TOKEN_RETURN,
  TOKEN_SUPER,
  TOKEN_THIS,
  TOKEN_TRUE,
  TOKEN_VAR,
  TOKEN_WHILE,

  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  int line;
  string_view str;
} Token;

class Scanner {
public:
  Scanner(std::string const &_src);
  Token scanToken();

private:
  std::string const &src;
  int start;
  int current;
  int line;

  Token string();
  Token number();
  Token identifier();
  Token makeToken(TokenType type);
  TokenType identifierType();

  bool isDigit(char);
  bool isAlpha(char);
  bool isAtEnd() { return this->current == int(this->src.length()); }
  char advance();
  bool match(char expected);
  Token errorToken(std::string s);
  void skipWhiteSpace();
  char peek();
  char peekNext();
};

} // namespace lox

#endif