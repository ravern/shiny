#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include "../bytecode.h"
#include "../frontend/string_interner.h"

// FUNCTIONS

struct Upvalue {
  int index;
  bool isLocal;
};

class FunctionObject {
 public:
  FunctionObject(uint8_t arity);
  FunctionObject(const FunctionObject& other);
  FunctionObject(FunctionObject&& other);
  ~FunctionObject() = default;

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

// OBJECTS

using Object = std::variant<FunctionObject>;