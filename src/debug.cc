#include "debug.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "runtime/object.h"

std::string opcodeToString(Opcode opcode) {
  switch (opcode) {
    case Opcode::NO_OP:
      return "NO_OP";
    case Opcode::NIL:
      return "NIL";
    case Opcode::TRUE:
      return "TRUE";
    case Opcode::FALSE:
      return "FALSE";
    case Opcode::CONST:
      return "CONST";
    case Opcode::CLOSURE:
      return "CLOSURE";
    case Opcode::ADD:
      return "ADD";
    case Opcode::SUB:
      return "SUB";
    case Opcode::MUL:
      return "MUL";
    case Opcode::DIV:
      return "DIV";
    case Opcode::MOD:
      return "MOD";
    case Opcode::NEG:
      return "NEG";
    case Opcode::EQ:
      return "EQ";
    case Opcode::NEQ:
      return "NEQ";
    case Opcode::LT:
      return "LT";
    case Opcode::LTE:
      return "LTE";
    case Opcode::GT:
      return "GT";
    case Opcode::GTE:
      return "GTE";
    case Opcode::AND:
      return "AND";
    case Opcode::OR:
      return "OR";
    case Opcode::NOT:
      return "NOT";
    case Opcode::BIT_AND:
      return "BIT_AND";
    case Opcode::BIT_OR:
      return "BIT_OR";
    case Opcode::BIT_XOR:
      return "BIT_XOR";
    case Opcode::BIT_NOT:
      return "BIT_NOT";
    case Opcode::SHIFT_LEFT:
      return "SHIFT_LEFT";
    case Opcode::SHIFT_RIGHT:
      return "SHIFT_RIGHT";
    case Opcode::LOAD:
      return "LOAD";
    case Opcode::STORE:
      return "STORE";
    case Opcode::DUP:
      return "DUP";
    case Opcode::POP:
      return "POP";
    case Opcode::TEST:
      return "TEST";
    case Opcode::JUMP:
      return "JUMP";
    case Opcode::CALL:
      return "CALL";
    case Opcode::RETURN:
      return "RETURN";
    case Opcode::HALT:
      return "HALT";
    case Opcode::UPVALUE_LOAD:
      return "UPVALUE_LOAD";
    case Opcode::UPVALUE_STORE:
      return "UPVALUE_STORE";
    case Opcode::UPVALUE_CLOSE:
      return "UPVALUE_CLOSE";
    case Opcode::MEMBER_GET:
      return "MEMBER_GET";
    case Opcode::MEMBER_SET:
      return "MEMBER_SET";
    case Opcode::GLOBAL_LOAD:
      return "GLOBAL_LOAD";
    case Opcode::GLOBAL_STORE:
      return "GLOBAL_STORE";
    default:
      return "<unknown>";
  }
}

std::string chunkToString(const Chunk& chunk, const std::string& name,
                          const StringInterner& stringInterner) {
  std::stringstream ss;

  ss << "== " << name << " ==\n";

  for (size_t offset = 0; offset < chunk.instructions.size(); ++offset) {
    ss << instructionToString(offset, chunk.instructions[offset],
                              stringInterner);
    ss << "\n";
  }

  return std::move(ss.str());
}

std::string instructionToString(size_t offset, Instruction instr,
                                const StringInterner& stringInterner) {
  std::stringstream ss;

  Opcode opcode = static_cast<Opcode>(instr & 0xFF);
  uint32_t operand = instr >> 8;

  ss << std::setw(4) << std::left << offset << "  ";
  std::string opname = opcodeToString(opcode);
  ss << std::setw(20) << std::left << opname;

  switch (opcode) {
    case Opcode::CONST:
    case Opcode::CLOSURE:
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::NEG:
    case Opcode::LT:
    case Opcode::LTE:
    case Opcode::GT:
    case Opcode::GTE:
    case Opcode::LOAD:
    case Opcode::STORE:
    case Opcode::CALL:
    case Opcode::JUMP:
    case Opcode::UPVALUE_LOAD:
    case Opcode::UPVALUE_STORE:
    case Opcode::GLOBAL_LOAD:
    case Opcode::GLOBAL_STORE:
    case Opcode::MEMBER_GET:
    case Opcode::MEMBER_SET: {
      ss << operand;
      break;
    }
    default:
      break;
  }

  return std::move(ss.str());
}

std::string valueToString(const Value& value,
                          const StringInterner& stringInterner) {
  std::stringstream ss;
  if (value.isNil()) {
    ss << "nil";
  } else if (value.isBool()) {
    ss << (value.asBool() ? "true" : "false");
  } else if (value.isInt()) {
    ss << value.asInt();
  } else if (value.isDouble()) {
    ss << value.asDouble();
  } else if (value.isObject<FunctionObject>()) {
    auto function = value.asObject<FunctionObject>();
    ss << (function->getName().has_value()
               ? stringInterner.get(function->getName().value())
               : "<anonymous>");
    ss << "@" << function.__getPtr();
  } else if (value.isObject<ClosureObject>()) {
    auto closure = value.asObject<ClosureObject>();
    ss << (closure->getFunction()->getName().has_value()
               ? stringInterner.get(closure->getFunction()->getName().value())
               : "<anonymous>");
    ss << "@" << closure.__getPtr();
  } else if (value.isObject<MethodObject>()) {
    auto method = value.asObject<MethodObject>();
    ss << (method->getFunction()->getName().has_value()
               ? stringInterner.get(method->getFunction()->getName().value())
               : "<anonymous>");
    ss << "@" << method.__getPtr();
  } else if (value.isObject<UpvalueObject>()) {
    auto upvalue = value.asObject<UpvalueObject>();
    ss << "(" << (upvalue->isOpen() ? "open" : "closed") << ","
       << (upvalue->isOpen()
               ? std::to_string(upvalue->getStackSlot())
               : valueToString(upvalue->getClosedValue(), stringInterner))
       << ")@" << upvalue.__getPtr();
  } else if (value.isObject<StringObject>()) {
    auto string = value.asObject<StringObject>();
    ss << "\"" << string->getData() << "\"";
  } else if (value.isObject<InstanceObject>()) {
    auto instance = value.asObject<InstanceObject>();
    ss << (instance->getClass()->getName().has_value()
               ? stringInterner.get(instance->getClass()->getName().value())
               : "<anonymous>")
       << "@" << instance.__getPtr();
  } else if (value.isObject<ClassObject>()) {
    auto klass = value.asObject<ClassObject>();
    ss << "class(";
    ss << (klass->getName().has_value()
               ? stringInterner.get(klass->getName().value())
               : "<anonymous>")
       << ")@" << klass.__getPtr();
  } else {
    throw std::runtime_error("Tried to convert unknown value type to string");
  }
  return std::move(ss.str());
}
