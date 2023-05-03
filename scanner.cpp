#include "scanner.h"

namespace lox {
Scanner::Scanner(std::string const &_src) : src{_src} {}

Token Scanner::scanToken() {
  skipWhiteSpace();
  start = current;
  if (isAtEnd()) {
    return makeToken(TOKEN_EOF);
  }

  char c = advance();
  if (isAlpha(c)) {
    return identifier();
  }
  if (isDigit(c)) {
    return number();
  }

  switch (c) {
  case '(':
    return makeToken(TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(TOKEN_RIGHT_PAREN);
  case '{':
    return makeToken(TOKEN_LEFT_BRACE);
  case '}':
    return makeToken(TOKEN_RIGHT_BRACE);
  case ';':
    return makeToken(TOKEN_SEMICOLON);
  case ',':
    return makeToken(TOKEN_COMMA);
  case '.':
    return makeToken(TOKEN_DOT);
  case '-':
    return makeToken(TOKEN_MINUS);
  case '+':
    return makeToken(TOKEN_PLUS);
  case '/':
    return makeToken(TOKEN_SLASH);
  case '*':
    return makeToken(TOKEN_STAR);
  case '!':
    return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<':
    return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case '"':
    return string();
  }

  return errorToken("Unexpected character.");
}

Token Scanner::identifier() {
  while (isAlpha(peek()) || isDigit(peek())) {
    advance();
  }
  return makeToken(identifierType());
}

Token Scanner::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }
    advance();
  }

  if (isAtEnd()) {
    return errorToken("Unterminated string.");
  }

  advance();
  return makeToken(TOKEN_STRING);
}

Token Scanner::number() {
  while (isDigit(peek())) {
    advance();
  }

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek())) {
      advance();
    }
  }

  return makeToken(TOKEN_NUMBER);
}

TokenType Scanner::identifierType() { return TOKEN_IDENTIFIER; }

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }
bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'b') || (c >= 'A' && c <= 'Z') || c == '_';
}

Token Scanner::makeToken(TokenType type) {
  Token t = {type, this->line,
             string_view{this->src.data(),
                         static_cast<size_t>(this->current - this->start)}};
  return t;
}

Token Scanner::errorToken(std::string s) {
  Token t = {TOKEN_ERROR, this->line, string_view{s}};
  return t;
}

char Scanner::advance() {
  this->current++;

  return this->src[this->current - 1];
}

bool Scanner::match(char expected) {
  if (isAtEnd()) {
    return false;
  }
  if (src[current] != expected) {
    return false;
  }
  current++;
  return true;
}

void Scanner::skipWhiteSpace() {
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
      advance();
      break;
    case '\n':
      line++;
      advance();
      break;
    case '/':
      if (peekNext() == '/') {
        while (peek() != '\n' && !isAtEnd()) {
          advance();
        }
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}

char Scanner::peek() { return src[current]; }
char Scanner::peekNext() {
  if (isAtEnd()) {
    return '\0';
  }
  return src[current + 1];
}

} // namespace lox