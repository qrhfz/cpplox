#ifndef cpplox_scanner_h
#define cpplox_scanner_h
#include <string>

namespace lox {
class Scanner {
public:
  Scanner(std::string const &src);

private:
  int start;
  int current;
  int line;
};

} // namespace lox

#endif