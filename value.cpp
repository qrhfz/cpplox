#include "value.h"

#include <iostream>
#include <variant>

namespace lox {
struct Printer {
  void operator()(double value) const { std::cout << value; }
  void operator()(bool value) const { std::cout << (value ? "true" : "false"); }
  void operator()(Nil) const { std::cout << "nil"; }
};

void printValue(Value value) { std::visit(Printer{}, value); }

} // namespace lox
