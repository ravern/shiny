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

class FunctionObject {
 public:
  FunctionObject(std::optional<SymbolId> name = std::nullopt) : name(name) {}
  ~FunctionObject() = default;

  std::optional<SymbolId> getName() const { return name; }
  Chunk& getChunk() { return chunk; }
  const Chunk& getChunk() const { return chunk; }
  std::vector<Upvalue>& getUpvalues() { return upvalues; }
  const std::vector<Upvalue>& getUpvalues() const { return upvalues; }

  int addUpvalue(Upvalue upvalue) {
    upvalues.push_back(upvalue);
    return upvalues.size() - 1;
  }

 private:
  Chunk chunk;
  std::vector<Upvalue> upvalues;
  std::optional<SymbolId> name;
};

class UpvalueObject {
 public:
  UpvalueObject(int stackSlot)
      : closedValue(Value::NIL),
        stackSlot(stackSlot),
        open(true),
        next(std::nullopt) {}
  UpvalueObject(int stackSlot, ObjectPtr<UpvalueObject> next)
      : closedValue(Value::NIL),
        stackSlot(stackSlot),
        open(true),
        next(std::move(next)) {}

  bool isOpen() const { return open; }

  void close(const std::vector<Value>& stack) {
    if (!open) {
      throw std::runtime_error("Tried to close already closed upvalue");
    }
    closedValue = getValue(stack);
    open = false;
  }

  int getStackSlot() const {
    if (!open) {
      throw std::runtime_error("Tried to get stack slot of open upvalue");
    }
    return stackSlot;
  }

  Value getClosedValue() const {
    if (open) {
      throw std::runtime_error("Tried to get closed value of open upvalue");
    }
    return closedValue;
  }

  Value getValue(const std::vector<Value>& stack) const {
    if (open) {
      return stack[stackSlot];
    } else {
      return closedValue;
    }
  }

  void setValue(const Value& value, std::vector<Value>& stack) {
    if (open) {
      stack[stackSlot] = value;
    } else {
      closedValue = value;
    }
  }

  std::optional<ObjectPtr<UpvalueObject>>& getNext() { return next; }
  const std::optional<ObjectPtr<UpvalueObject>>& getNext() const {
    return next;
  }

 private:
  Value closedValue;
  int stackSlot;
  bool open;
  std::optional<ObjectPtr<UpvalueObject>> next;
};

class ClosureObject {
 public:
  ClosureObject(ObjectPtr<FunctionObject> function)
      : function(std::move(function)) {}
  ClosureObject(ObjectPtr<FunctionObject> function,
                std::vector<ObjectPtr<UpvalueObject>>&& upvalues)
      : function(std::move(function)), upvalues(std::move(upvalues)) {}

  ObjectPtr<FunctionObject>& getFunction() { return function; }
  const ObjectPtr<FunctionObject>& getFunction() const { return function; }
  std::vector<ObjectPtr<UpvalueObject>>& getUpvalues() { return upvalues; }
  const std::vector<ObjectPtr<UpvalueObject>>& getUpvalues() const {
    return upvalues;
  }
  ObjectPtr<UpvalueObject>& getUpvalue(int index) { return upvalues[index]; }
  const ObjectPtr<UpvalueObject>& getUpvalue(int index) const {
    return upvalues[index];
  }

 private:
  ObjectPtr<FunctionObject> function;
  std::vector<ObjectPtr<UpvalueObject>> upvalues;
};

class MethodObject {
 public:
  MethodObject(ObjectPtr<FunctionObject> function, Value self)
      : function(std::move(function)), self(std::move(self)) {}

  ObjectPtr<FunctionObject>& getFunction() { return function; }
  const ObjectPtr<FunctionObject>& getFunction() const { return function; }
  Value& getSelf() { return self; }
  const Value& getSelf() const { return self; }

 private:
  ObjectPtr<FunctionObject> function;
  Value self;
};

class ClassObject {
 public:
  ClassObject(ObjectPtr<ClassObject> superklass, SymbolId name)
      : superklass(std::move(superklass)), name(name) {}
  ClassObject(SymbolId name) : superklass(std::nullopt), name(name) {}
  ClassObject() : superklass(std::nullopt), name(std::nullopt) {}

  std::optional<ObjectPtr<ClassObject>>& getSuperklass() { return superklass; }
  const std::optional<ObjectPtr<ClassObject>>& getSuperklass() const {
    return superklass;
  }
  std::optional<SymbolId>& getName() { return name; }
  const std::optional<SymbolId>& getName() const { return name; }
  std::vector<Value>& getMembers() { return members; }
  const std::vector<Value>& getMembers() const { return members; }
  Value getMember(int index) const { return members[index]; }

 private:
  std::optional<ObjectPtr<ClassObject>> superklass;
  std::optional<SymbolId> name;
  std::vector<Value> members;
};

class InstanceObject {
 public:
  InstanceObject(ObjectPtr<ClassObject> klass) : klass(std::move(klass)) {
    members.resize(klass->getMembers().size());
  }

  ObjectPtr<ClassObject>& getClass() { return klass; }
  const ObjectPtr<ClassObject>& getClass() const { return klass; }
  std::vector<Value>& getMembers() { return members; }
  const std::vector<Value>& getMembers() const { return members; }
  Value getMember(int index) const { return members[index]; }
  void setMember(int index, Value newValue) { members[index] = newValue; }

 private:
  ObjectPtr<ClassObject> klass;
  std::vector<Value> members;
};

class StringObject {
 public:
  StringObject(std::string data) : data(std::move(data)) {}

  std::string& getData() { return data; }
  const std::string& getData() const { return data; }

 private:
  std::string data;
};

class ArrayObject {
 public:
  ArrayObject();
  ~ArrayObject() = default;

  std::vector<Value>& getValues() { return values; }
  const std::vector<Value>& getValues() const { return values; }
  Value get(int index) const { return values[index]; }
  void set(int index, Value newValue) { values[index] = newValue; }
  void append(Value value) { values.push_back(value); }

 private:
  std::vector<Value> values;
};

class DictObject {
 public:
  DictObject();
  ~DictObject() = default;

  std::unordered_map<Value, Value>& getValues() { return values; }
  const std::unordered_map<Value, Value>& getValues() const { return values; }
  Value get(const Value& key) const { return values.at(key); }
  void set(const Value& key, Value newValue) { values[key] = newValue; }

 private:
  std::unordered_map<Value, Value> values;
};

class BuiltInObject {
 public:
  BuiltInObject(
      std::function<Value(std::vector<Value>&, StringInterner&)> function,
      std::optional<std::string> name = std::nullopt)
      : function(std::move(function)), name(std::move(name)) {}

  const std::optional<std::string>& getName() const { return name; }
  Value call(std::vector<Value>& args, StringInterner& stringInterner) {
    return function(args, stringInterner);
  }

 private:
  std::optional<std::string> name;
  std::function<Value(std::vector<Value>&, StringInterner&)> function;
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

  std::variant<FunctionObject, UpvalueObject, ClosureObject, StringObject,
               ArrayObject, DictObject, MethodObject, ClassObject,
               InstanceObject, BuiltInObject>
      data;
  int strongCount;
  int weakCount;
};
