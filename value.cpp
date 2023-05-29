#include "value.h"

#include <iostream>
#include <variant>

namespace lox {
struct Printer {
  void operator()(double value) const { std::cout << value; }
  void operator()(bool value) const { std::cout << (value ? "true" : "false"); }
  void operator()(LoxNil value) const { std::cout << "nil"; }
};

void printValue(Value value) {
  static const Printer printer{};

  std::visit(printer, value);
}

} // namespace lox
