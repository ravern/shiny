#pragma once

#include <vector>

#include "../runtime/object.h"
#include "../runtime/object_ptr.h"
#include "../runtime/value.h"

struct Frame {
  ObjectPtr<ClosureObject> closure;
  int ip;
  int bp;
};

class VM {
 public:
  VM();

  void evaluate(ObjectPtr<FunctionObject> function);

 private:
  ObjectPtr<ClosureObject> closure;
  int ip;
  int bp;
  std::vector<Value> stack;
  std::vector<Frame> callStack;
  std::vector<ObjectPtr<UpvalueObject>> upvalueStack;
};