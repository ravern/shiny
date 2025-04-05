#pragma once

#include <cstdint>
#include <vector>

#include "runtime/value.h"

// Shiny uses a 32-bit instruction format. The first byte is always the opcode.
// The remaining 3 bytes are used to specify the operand. If the instruction
// does not have an operand, the remaining bytes should be set to 0.
using Instruction = uint32_t;

enum class Opcode : uint8_t {
  NO_OP = 0x00,

  NIL = 0x11,
  TRUE = 0x12,
  FALSE = 0x13,
  ARRAY = 0x14,
  DICT = 0x15,
  CONST = 0x16,     // operand: index of constant
  CLOSURE = 0x17,   // operand: index of function definition
  BUILT_IN = 0x18,  // operand: index of built-in function

  ADD = 0x31,  // operand: type of operands
  SUB = 0x32,  // operand: type of operands
  MUL = 0x33,  // operand: type of operands
  DIV = 0x34,  // operand: type of operands
  MOD = 0x35,
  NEG = 0x36,  // operand: type of operands
  EQ = 0x37,
  NEQ = 0x38,
  LT = 0x39,   // operand: type of operands
  LTE = 0x3a,  // operand: type of operands
  GT = 0x3b,   // operand: type of operands
  GTE = 0x3c,  // operand: type of operands
  AND = 0x3d,
  OR = 0x3e,
  NOT = 0x3f,

  BIT_AND = 0x40,
  BIT_OR = 0x41,
  BIT_XOR = 0x42,
  BIT_NOT = 0x43,
  SHIFT_LEFT = 0x44,
  SHIFT_RIGHT = 0x45,

  LOAD = 0x50,   // operand: stack slot of local
  STORE = 0x51,  // operand: stack slot of local
  DUP = 0x52,
  POP = 0x53,

  TEST = 0x60,
  JUMP = 0x61,       // operand: offset of instruction to jump to
  CALL = 0x62,       // operand: number of arguments
  TAIL_CALL = 0x63,  // operand: number of arguments
  RETURN = 0x64,
  HALT = 0x65,

  UPVALUE_LOAD = 0x70,   // operand: index of upvalue
  UPVALUE_STORE = 0x71,  // operand: index of upvalue
  UPVALUE_CLOSE = 0x72,

  OBJECT_GET_MEMBER = 0x81,  // operand: index of member
  OBJECT_SET_MEMBER = 0x82,  // operand: index of member
  OBJECT_GET_METHOD = 0x83,  // operand: index of method definition

  GLOBAL_LOAD = 0x91,   // operand: index of global
  GLOBAL_STORE = 0x92,  // operand: index of global
};

struct Chunk {
  std::vector<Instruction> instructions;
  std::vector<Value> constants;

  // for error reporting and debugging
  // TODO: add vector of line-cols where each instruction is defined
};
