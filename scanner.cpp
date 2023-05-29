#include "scanner.h"

namespace lox {
Scanner::Scanner(std::string const &_src) : src{_src} {
  start = 0;
  current = 0;
  line = 1;
}

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

TokenType Scanner::identifierType() {
  switch (src[start]) {
  case 'a':
    return checkKeyword(1, "nd", TOKEN_AND);
  case 'c':
    return checkKeyword(1, "lass", TOKEN_CLASS);
  case 'e':
    return checkKeyword(1, "lse", TOKEN_ELSE);
  case 'i':
    return checkKeyword(1, "f", TOKEN_IF);
  case 'n':
    return checkKeyword(1, "il", TOKEN_NIL);
  case 'o':
    return checkKeyword(1, "r", TOKEN_OR);
  case 'p':
    return checkKeyword(1, "rint", TOKEN_PRINT);
  case 'r':
    return checkKeyword(1, "eturn", TOKEN_RETURN);
  case 's':
    return checkKeyword(1, "uper", TOKEN_SUPER);
  case 'v':
    return checkKeyword(1, "ar", TOKEN_VAR);
  case 'w':
    return checkKeyword(1, "hile", TOKEN_WHILE);
  case 'f':
    if (current - start > 1) {
      switch (src[start + 1]) {
      case 'a':
        return checkKeyword(2, "lse", TOKEN_FALSE);
      case 'o':
        return checkKeyword(2, "r", TOKEN_FOR);
      case 'u':
        return checkKeyword(2, "n", TOKEN_FUN);
      }
    }
    break;
  case 't':
    if (current - start > 1) {
      switch (src[start + 1]) {
      case 'h':
        return checkKeyword(2, "is", TOKEN_THIS);
      case 'r':
        return checkKeyword(2, "ue", TOKEN_TRUE);
      }
    }
    break;
  }

  return TOKEN_IDENTIFIER;
}

TokenType Scanner::checkKeyword(int offset, std::string const &rest,
                                TokenType type) {
  int identifierLength = offset + rest.length();
  bool strEqual = rest.compare(src.substr(offset, rest.length())) == 0;
  if (current - start == identifierLength && strEqual) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }
bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

Token Scanner::makeToken(TokenType type) {
  Token t = {type, this->line,
             this->src.substr(this->start, this->current - this->start)};
  return t;
}

Token Scanner::errorToken(std::string s) {
  Token t = {TOKEN_ERROR, this->line, s};
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