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

  std::optional<SymbolId> getName() const;
  Chunk& getChunk();
  const Chunk& getChunk() const;
  std::vector<Upvalue>& getUpvalues();
  const std::vector<Upvalue>& getUpvalues() const;

  int addUpvalue(Upvalue upvalue);

 private:
  Chunk chunk;
  std::vector<Upvalue> upvalues;
  std::optional<SymbolId> name;
};

class UpvalueObject {
 public:
  UpvalueObject(Value* value);
  UpvalueObject(Value* value, ObjectPtr<UpvalueObject> next);
  ~UpvalueObject() = default;

  void close();

  Value* getValue() const;
  std::optional<ObjectPtr<UpvalueObject>>& getNext();
  const std::optional<ObjectPtr<UpvalueObject>>& getNext() const;

 private:
  Value closedValue;
  Value* value;
  std::optional<ObjectPtr<UpvalueObject>> next;
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
  ObjectPtr<UpvalueObject>& getUpvalue(int index);
  const ObjectPtr<UpvalueObject>& getUpvalue(int index) const;

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

  template <>
  std::monostate* get() {
    return nullptr;
  }

  std::variant<FunctionObject, UpvalueObject, ClosureObject> data;
  int strongCount;
  int weakCount;
};
