#include "object.h"

Object::Object(ObjectType kind) : type(kind) {}

ObjectType Object::getType() const { return type; }

// FUNCTIONS

FunctionObject::FunctionObject(uint8_t arity)
    : Object(ObjectType::Function), arity(arity) {}

uint8_t FunctionObject::getArity() const { return arity; }
const Chunk& FunctionObject::getChunk() const { return chunk; }
Chunk& FunctionObject::getChunk() { return chunk; }

const std::vector<Upvalue>& FunctionObject::getUpvalues() const {
  return upvalues;
}

int FunctionObject::addUpvalue(Upvalue upvalue) {
  for (int i = 0; i < upvalues.size(); i++) {
    if (upvalues[i].index == upvalue.index &&
        upvalues[i].isLocal == upvalue.isLocal) {
      return i;
    }
  }

  upvalues.push_back(upvalue);
  return upvalues.size() - 1;
}
