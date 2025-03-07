#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace Shiny {

// Shiny uses a 32-bit instruction format. The first byte is always the opcode.
// The remaining 3 bytes are used to specify the operand. If the instruction
// does not have an operand, the remaining bytes should be set to 0.
using Instruction = uint32_t;

enum class Opcode : uint8_t {
  NO_OP = 0x00,

  NIL = 0x11,
  TRUE = 0x12,
  FALSE = 0x13,
  CONST = 0x14,  // operand: index of constant

  ADD = 0x31,
  SUB = 0x32,
  MUL = 0x33,
  DIV = 0x34,
  MOD = 0x35,
  EQ = 0x36,
  NEQ = 0x37,
  LT = 0x38,
  LTE = 0x39,
  GT = 0x3a,
  GTE = 0x3b,
  AND = 0x3c,
  OR = 0x3d,
  NOT = 0x3e,

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

  UPVALUE_LOAD = 0x70,   // operand: index of upvalue
  UPVALUE_STORE = 0x71,  // operand: index of upvalue
  UPVALUE_CLOSE = 0x72,

  OBJECT_NEW = 0x80,
  OBJECT_GET_MEMBER = 0x81,   // operand: index of member
  OBJECT_SET_MEMBER = 0x82,   // operand: index of member
  OBJECT_CALL_METHOD = 0x83,  // operand: index of method
};

using Constant = std::variant<int64_t, double, std::string_view>;

struct Chunk {
  std::vector<Instruction> instructions;

  // for error reporting and debugging
  // TODO: add vector of line-cols where each instruction is defined
};

struct UpvalueDef {
  int index;
  bool isLocal;
};

struct FunctionDef {
  int parameterCount;
  std::shared_ptr<Chunk> body;
  std::optional<std::vector<UpvalueDef>> upvalues;

  // for error reporting and debugging
  std::string_view name;
  // TODO: add the line-col where the function is defined
};

struct ClassDef {
  int memberCount;
  std::vector<FunctionDef> methods;

  // for error reporting and debugging
  std::string_view name;
  // TODO: add the line-col where the class is defined
};

struct Program {
  std::vector<Constant> constants;
  std::vector<ClassDef> classDefs;
  std::vector<FunctionDef> functionDefs;
  std::vector<std::shared_ptr<Chunk>> chunks;
};

}  // namespace Shiny
