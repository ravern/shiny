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
  void pushFrame(int arity);
  void popFrame();
  ObjectPtr<UpvalueObject> captureUpvalue(Upvalue functionUpvalue);
  ObjectPtr<UpvalueObject> pushUpvalue(Value* value);
  void closeUpvalues(Value* upTillValue);

  ObjectPtr<ClosureObject> closure;
  int ip;
  int bp;
  Chunk* chunk;
  std::vector<Value> stack;
  std::vector<Frame> callStack;
  std::optional<ObjectPtr<UpvalueObject>> upvalueStack;
};