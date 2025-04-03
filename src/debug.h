#pragma once

#include <iomanip>
#include <iostream>
#include <string>

#include "bytecode.h"
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
    case Opcode::ARRAY:
      return "ARRAY";
    case Opcode::DICT:
      return "DICT";
    case Opcode::CONST:
      return "CONST";
    case Opcode::CLOSURE:
      return "CLOSURE";
    case Opcode::BUILT_IN:
      return "BUILT_IN";
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
    case Opcode::TAIL_CALL:
      return "TAIL_CALL";
    case Opcode::RETURN:
      return "RETURN";
    case Opcode::UPVALUE_LOAD:
      return "UPVALUE_LOAD";
    case Opcode::UPVALUE_STORE:
      return "UPVALUE_STORE";
    case Opcode::UPVALUE_CLOSE:
      return "UPVALUE_CLOSE";
    case Opcode::OBJECT_GET_MEMBER:
      return "OBJECT_GET_MEMBER";
    case Opcode::OBJECT_SET_MEMBER:
      return "OBJECT_SET_MEMBER";
    case Opcode::OBJECT_GET_METHOD:
      return "OBJECT_GET_METHOD";
    default:
      return "UNKNOWN";
  }
}

void disassembleChunk(const Chunk& chunk, const std::string& name) {
  std::cout << "== " << name << " ==\n";

  for (size_t offset = 0; offset < chunk.instructions.size(); ++offset) {
    Instruction instr = chunk.instructions[offset];
    Opcode opcode = static_cast<Opcode>(instr & 0xFF);
    uint32_t operand = instr >> 8;

    std::cout << std::setw(4) << std::left << offset << "  ";
    std::string opname = opcodeToString(opcode);
    std::cout << std::setw(24) << std::left << opname;

    switch (opcode) {
      case Opcode::CONST:
      case Opcode::CLOSURE:
      case Opcode::BUILT_IN:
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
      case Opcode::TAIL_CALL:
      case Opcode::JUMP:
      case Opcode::UPVALUE_LOAD:
      case Opcode::UPVALUE_STORE:
      case Opcode::OBJECT_GET_MEMBER:
      case Opcode::OBJECT_SET_MEMBER:
      case Opcode::OBJECT_GET_METHOD: {
        std::cout << operand;
        switch (opcode) {
          case Opcode::ADD:
          case Opcode::SUB:
          case Opcode::MUL:
          case Opcode::DIV:
          case Opcode::NEG:
          case Opcode::LT:
          case Opcode::LTE:
          case Opcode::GT:
          case Opcode::GTE: {
            switch (operand) {
              case 1:
                std::cout << " (int)";
                break;
              case 2:
                std::cout << " (double)";
                break;
              case 3:
                std::cout << " (string)";
                break;
              case 4:
                std::cout << " (array)";
                break;
              default:
                std::cout << " (unknown)";
                break;
            }
          }

          default:
            break;
        }
        break;
      }

      default:
        break;
    }

    std::cout << "\n";
  }
}
