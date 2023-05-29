#include "value.h"

#include <iostream>
#include <variant>

namespace lox {
void printValue(Value value) {
  struct Printer {
    void operator()(double value) const { std::cout << value; }
    void operator()(bool value) const {
      std::cout << (value ? "true" : "false");
    }
    void operator()(LoxNil) const { std::cout << "nil"; }
  };

  static const Printer printer{};

  std::visit(printer, value);
}

} // namespace lox
