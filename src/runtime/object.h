#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include "../bytecode.h"
#include "../frontend/string_interner.h"
#include "object_ptr.h"

// FUNCTION

struct Upvalue {
  int index;
  bool isLocal;
};

class FunctionObject {
 public:
  FunctionObject(uint8_t arity);
  FunctionObject(SymbolId name, uint8_t arity);
  FunctionObject(const FunctionObject& other);
  FunctionObject(FunctionObject&& other);
  ~FunctionObject() = default;

 public:
  uint8_t getArity() const;
  const std::vector<Upvalue>& getUpvalues() const;
  const Chunk& getChunk() const;
  Chunk& getChunk();
  SymbolId getName() const;

 public:
  int addUpvalue(Upvalue upvalue);

 private:
  uint8_t arity;
  std::vector<Upvalue> upvalues;
  Chunk chunk;

  // for error reporting and debugging
  SymbolId name;
};

// UPVALUE

class UpvalueObject {
 public:
  UpvalueObject(Value closedValue, Value* value);
  ~UpvalueObject() = default;

 private:
  Value closedValue;
  Value* value;
};

// CLOSURE

class ClosureObject {
 public:
  ClosureObject(ObjectPtr<FunctionObject>&& function,
                std::vector<ObjectPtr<UpvalueObject>>&& upvalues);
  ~ClosureObject() = default;

 public:
  const std::vector<ObjectPtr<UpvalueObject>>& getUpvalues() const;
  std::vector<ObjectPtr<UpvalueObject>>& getUpvalues();
  const FunctionObject& getFunction() const;
  FunctionObject& getFunction();

 private:
  ObjectPtr<FunctionObject> function;
  std::vector<ObjectPtr<UpvalueObject>> upvalues;
};

// OBJECT

struct Object {
  std::variant<FunctionObject, ClosureObject, UpvalueObject> data;
  int strongCount;
  int weakCount;
};