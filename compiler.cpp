#include "compiler.h"
#include "scanner.h"
#include <cstdio>
#include <string>

using std::printf;

namespace lox {
void compile(std::string const &src) {
  Scanner scanner{src};
  int line = -1;
  for (;;) {
    Token token = scanner.scanToken();
    if (token.line != line) {
      printf("%4d ", token.line);
      line = token.line;
    } else {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.type, int(token.str.length()),
           token.str.data());

    if (token.type == TOKEN_EOF) {
      break;
    }
  }
}
} // namespace lox