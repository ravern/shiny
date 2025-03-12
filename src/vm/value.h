#pragma once

#include <cstdint>

namespace Shiny {

// NaN-boxed 64-bit value
struct Value {
  uint64_t raw;
};

}  // namespace Shiny