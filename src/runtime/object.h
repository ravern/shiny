#pragma once

#include <variant>

#include "../bytecode.h"
#include "../frontend/string_interner.h"
#include "object_ptr.h"

class Value;

struct Upvalue {
  int index;
  bool isLocal;
};

class FunctionObject {
 public:
  FunctionObject();
  ~FunctionObject() = default;

  Chunk& getChunk();
  const Chunk& getChunk() const;
  std::vector<Upvalue>& getUpvalues();
  const std::vector<Upvalue>& getUpvalues() const;

  int addUpvalue(Upvalue upvalue);

 private:
  Chunk chunk;
  std::vector<Upvalue> upvalues;
  SymbolId name;
};

class UpvalueObject {
 public:
  UpvalueObject(Value* value);
  ~UpvalueObject() = default;

 private:
  Value closedValue;
  Value* value;
};

class ClosureObject {
 public:
  ClosureObject(ObjectPtr<FunctionObject> function);
  ClosureObject(ObjectPtr<FunctionObject> function,
                std::vector<ObjectPtr<UpvalueObject>>&& upvalues);
  ~ClosureObject() = default;

  ObjectPtr<FunctionObject>& getFunction();
  const ObjectPtr<FunctionObject>& getFunction() const;
  std::vector<ObjectPtr<UpvalueObject>>& getUpvalues();
  const std::vector<ObjectPtr<UpvalueObject>>& getUpvalues() const;

 private:
  ObjectPtr<FunctionObject> function;
  std::vector<ObjectPtr<UpvalueObject>> upvalues;
};

class Object {
 public:
  template <typename T>
  Object(T&& o) : data(std::move(o)), strongCount(1), weakCount(0) {}

  ~Object() = default;

  template <typename T>
  T* get() {
    return &std::get<T>(data);
  }

  std::variant<FunctionObject, UpvalueObject, ClosureObject> data;
  int strongCount;
  int weakCount;
};
