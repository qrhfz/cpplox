#ifndef clox_common_h
#define clox_common_h

#include <cstddef>
#include <cstdint>

#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE

namespace lox {
template <typename T> T nextEnum(T enumMember) {
  return static_cast<T>(static_cast<int>(enumMember) + 1);
}

} // namespace lox

#endif