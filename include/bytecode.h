#ifndef SHINY_BYTECODE_H
#define SHINY_BYTECODE_H

#include <cstdint>

namespace Shiny {

enum class Opcode : uint8_t {
  ADD = 0x11,
  SUB = 0x12,
  MUL = 0x13,
  DIV = 0x14,
};

}  // namespace Shiny

#endif