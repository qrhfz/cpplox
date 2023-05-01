#include "compiler.h"
#include "scanner.h"
#include <string>

namespace lox {
void compile(std::string const &src) { Scanner scanner{src}; }
} // namespace lox