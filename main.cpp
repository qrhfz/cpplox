#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <fstream>

static void repl(vm::VM &);
static void runFile(vm::VM &, char *const);

int main(int argc, char **argv) {
  vm::VM vm{};

  if (argc == 1) {
    repl(vm);
  } else if (argc == 2) {
    runFile(vm, argv[1]);
  } else {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  return 0;
}

static void repl(vm::VM &vm) {
  std::string line;

  for (;;) {
    std::cout << "> ";
    std::getline(std::cin, line);

    if (line.empty()) {
      std::cout << "\n";
      break;
    }

    vm.interpret(line);
  }
}

static void runFile(vm::VM &vm, char *const path) {
  std::stringstream buffer;
  std::ifstream srcFile{path}; // open the file for reading
  if (!srcFile.is_open()) {
    std::cerr << "Unable to open file\n";
    exit(74);
  }

  buffer << srcFile.rdbuf();
  std::string src = buffer.str();
  srcFile.close(); // close the file

  vm.interpret(src);
}