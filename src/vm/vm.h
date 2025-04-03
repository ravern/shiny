#pragma once

#include "../runtime/value.h"

struct Frame {
  ObjectRef function;
  int ip;
  int bp;
};

class VM {
 public:
  Value evaluate(ObjectRef function);
};