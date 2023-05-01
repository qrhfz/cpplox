#ifndef brainfuck_scanner_h
#define brainfuck_scanner_h
#include <string>

class Scanner {
public:
  Scanner(std::string const &src);

private:
  int start;
  int current;
  int line;
};

#endif