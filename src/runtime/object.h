#pragma once

#include <cstdint>
#include <vector>

#include "../bytecode.h"
#include "../frontend/string_interner.h"

enum class ObjectType {
  Function,
};

class Object {
 public:
  Object() = delete;
  virtual ~Object() = default;

 public:
  ObjectType getType() const;

 protected:
  explicit Object(ObjectType kind);

 private:
  ObjectType type;
};

// FUNCTIONS

struct Upvalue {
  int index;
  bool isLocal;
};

class FunctionObject : public Object {
 public:
  FunctionObject(uint8_t arity);

 public:
  uint8_t getArity() const;
  const std::vector<Upvalue>& getUpvalues() const;
  const Chunk& getChunk() const;
  Chunk& getChunk();

 public:
  int addUpvalue(Upvalue upvalue);

 private:
  uint8_t arity;
  std::vector<Upvalue> upvalues;
  Chunk chunk;

  // for error reporting and debugging
  SymbolId name;
};
