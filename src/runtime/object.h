#pragma once

#include <optional>
#include <unordered_map>
#include <variant>

#include "../bytecode.h"
#include "../frontend/string_interner.h"
#include "object_ptr.h"

class Value;

struct Upvalue {
  int index;
  bool isLocal;
};

class ClassObject {

};

class InstanceObject {
 ClassObject& klass;
 std::vector<Value> fields;
};

class FunctionObject {
 public:
  FunctionObject(std::optional<SymbolId> name = std::nullopt);
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
  UpvalueObject(int stackSlot);
  UpvalueObject(int stackSlot, ObjectPtr<UpvalueObject> next);
  ~UpvalueObject() = default;

  void close(const std::vector<Value>& stack);

  bool isOpen() const;
  int getStackSlot() const;
  Value getClosedValue() const;
  Value getValue(const std::vector<Value>& stack) const;
  void setValue(const Value& value, std::vector<Value>& stack);
  std::optional<ObjectPtr<UpvalueObject>>& getNext();
  const std::optional<ObjectPtr<UpvalueObject>>& getNext() const;

 private:
  Value closedValue;
  int stackSlot;
  bool open;
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

class MethodObject {
 public:
  MethodObject(ObjectPtr<FunctionObject> function, Value self);
  ~MethodObject() = default;

 private:
  ObjectPtr<FunctionObject> function;
  Value self;
};

class ClassObject {
 public:
  ClassObject(ObjectPtr<ClassObject> superklass,
              std::optional<SymbolId> name = std::nullopt);
  ~ClassObject() = default;

 private:
  ObjectPtr<ClassObject> superklass;
  std::optional<SymbolId> name;
  std::vector<ObjectPtr<FunctionObject>> methods;
};

class InstanceObject {
 public:
  InstanceObject(ObjectPtr<ClassObject> klass);
  ~InstanceObject() = default;

 private:
  ObjectPtr<ClassObject> klass;
  std::vector<Value> fields;
};

class ArrayObject {
 public:
  ArrayObject();
  ~ArrayObject() = default;

 private:
  std::vector<Value> values;
};

class DictObject {
 public:
  DictObject();
  ~DictObject() = default;

 private:
  std::unordered_map<Value, Value> values;
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

  template <typename T>
  bool is() const {
    return std::holds_alternative<T>(data);
  }

  template <>
  bool is<std::monostate>() const {
    return false;
  }

  std::variant<FunctionObject, UpvalueObject, ClosureObject, ArrayObject> data;
  int strongCount;
  int weakCount;
};
