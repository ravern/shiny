#pragma once

#include "../bytecode.h"
#include "../runtime/value.h"

namespace Shiny {

class VM {
 public:
  Value evaluate(Program& program);

  // disable copying
  VM(const VM&) = delete;
  VM& operator=(const VM&) = delete;
};

}  // namespace Shiny