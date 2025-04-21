#pragma once

#include <vector>

#include "../runtime/object.h"
#include "../runtime/object_ptr.h"
#include "../runtime/value.h"

struct Frame {
  Value function;
  int ip;
  int bp;
};

class VM {
 public:
  VM(StringInterner& stringInterner, bool verbose = false);
  VM(StringInterner& stringInterner, const std::vector<Value>& globals,
     bool verbose = false);

  Value evaluate(ObjectPtr<FunctionObject> function);

 private:
  void callClass();
  void pushFrame(int arity);
  void popFrame();
  ObjectPtr<UpvalueObject> captureUpvalue(Upvalue functionUpvalue);
  ObjectPtr<UpvalueObject> pushUpvalue(int stackSlot);
  void closeUpvalues(int upTillStackSlot);
  void printStack();
  void printUpvalueStack();
  ObjectPtr<FunctionObject> getFunctionFromValue(Value value);

  StringInterner& stringInterner;
  Value currentFunction;
  int ip;
  int bp;
  Chunk* chunk;
  std::vector<Value> globals;
  std::vector<Value> stack;
  std::vector<Frame> callStack;
  std::optional<ObjectPtr<UpvalueObject>> upvalueStack;
  Value lastPoppedValue;
  bool verbose;
};