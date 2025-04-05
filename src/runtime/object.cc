#include "object.h"

FunctionObject::FunctionObject() : name(std::nullopt) {}

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

void UpvalueObject::close() {
  closedValue = *value;
  value = &closedValue;
}

Value* UpvalueObject::getValue() const { return value; }

std::optional<ObjectPtr<UpvalueObject>>& UpvalueObject::getNext() {
  return next;
}

const std::optional<ObjectPtr<UpvalueObject>>& UpvalueObject::getNext() const {
  return next;
}

UpvalueObject::UpvalueObject(Value* value)
    : closedValue(Value::NIL), value(value), next(std::nullopt) {}

UpvalueObject::UpvalueObject(Value* value, ObjectPtr<UpvalueObject> next)
    : closedValue(Value::NIL), value(value), next(std::move(next)) {}

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