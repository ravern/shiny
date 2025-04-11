#include "object.h"

FunctionObject::FunctionObject(std::optional<SymbolId> name) : name(name) {}

std::optional<SymbolId> FunctionObject::getName() const { return name; }

Chunk& FunctionObject::getChunk() { return chunk; }

const Chunk& FunctionObject::getChunk() const { return chunk; }

std::vector<Upvalue>& FunctionObject::getUpvalues() { return upvalues; }

const std::vector<Upvalue>& FunctionObject::getUpvalues() const {
  return upvalues;
}

int FunctionObject::addUpvalue(Upvalue upvalue) {
  upvalues.push_back(upvalue);
  return upvalues.size() - 1;
}

UpvalueObject::UpvalueObject(int stackSlot)
    : closedValue(Value::NIL),
      stackSlot(stackSlot),
      open(true),
      next(std::nullopt) {}

UpvalueObject::UpvalueObject(int stackSlot, ObjectPtr<UpvalueObject> next)
    : closedValue(Value::NIL),
      stackSlot(stackSlot),
      open(true),
      next(std::move(next)) {}

void UpvalueObject::close(const std::vector<Value>& stack) {
  if (open) {
    closedValue = getValue(stack);
    open = false;
  }
}

bool UpvalueObject::isOpen() const { return open; }

int UpvalueObject::getStackSlot() const {
  if (!open) {
    throw std::runtime_error("Tried to get stack slot of open upvalue");
  }
  return stackSlot;
}

Value UpvalueObject::getClosedValue() const {
  if (open) {
    throw std::runtime_error("Tried to get closed value of open upvalue");
  }
  return closedValue;
}

void UpvalueObject::setValue(const Value& value, std::vector<Value>& stack) {
  if (open) {
    stack[stackSlot] = value;
  } else {
    closedValue = value;
  }
}

Value UpvalueObject::getValue(const std::vector<Value>& stack) const {
  if (open) {
    return stack[stackSlot];
  } else {
    return closedValue;
  }
}

std::optional<ObjectPtr<UpvalueObject>>& UpvalueObject::getNext() {
  return next;
}

const std::optional<ObjectPtr<UpvalueObject>>& UpvalueObject::getNext() const {
  return next;
}

ClosureObject::ClosureObject(ObjectPtr<FunctionObject> function)
    : function(std::move(function)) {
  upvalues.resize(this->function->getUpvalues().size());
}

ClosureObject::ClosureObject(ObjectPtr<FunctionObject> function,
                             std::vector<ObjectPtr<UpvalueObject>>&& upvalues)
    : function(std::move(function)), upvalues(std::move(upvalues)) {}

ObjectPtr<FunctionObject>& ClosureObject::getFunction() { return function; }

const ObjectPtr<FunctionObject>& ClosureObject::getFunction() const {
  return function;
}

std::vector<ObjectPtr<UpvalueObject>>& ClosureObject::getUpvalues() {
  return upvalues;
}

const std::vector<ObjectPtr<UpvalueObject>>& ClosureObject::getUpvalues()
    const {
  return upvalues;
}

ObjectPtr<UpvalueObject>& ClosureObject::getUpvalue(int index) {
  return upvalues[index];
}

const ObjectPtr<UpvalueObject>& ClosureObject::getUpvalue(int index) const {
  return upvalues[index];
}

MethodObject::MethodObject(ObjectPtr<FunctionObject> function, Value self)
    : function(std::move(function)), self(std::move(self)) {}

ClassObject::ClassObject(ObjectPtr<ClassObject> superklass, SymbolId name)
    : superklass(std::move(superklass)), name(name) {}

ClassObject::ClassObject(SymbolId name)
    : superklass(std::nullopt), name(name) {}

ClassObject::ClassObject() : superklass(std::nullopt), name(std::nullopt) {}

InstanceObject::InstanceObject(ObjectPtr<ClassObject> klass)
    : klass(std::move(klass)) {}