#include "object.h"

FunctionObject::FunctionObject() : name(34) {}

SymbolId FunctionObject::getName() const { return name; }

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

UpvalueObject::UpvalueObject(Value* value) : value(value) {}

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
