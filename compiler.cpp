#include "compiler.h"
#include "scanner.h"
#include <string>

namespace compiler {
void compile(std::string const &src) { Scanner scanner{src}; }
} // namespace compiler