#include "vm.h"

#include <stdexcept>

#include "../runtime/object.h"

Value VM::evaluate(ObjectRef function) {
  Chunk& chunk = function.toFunction().getChunk();
  int ip = 0;
  int bp = 0;
  std::vector<Value> stack;
  std::vector<Frame> callStack;

  while (true) {
    Instruction instruction = chunk.instructions[ip++];
    Opcode opcode = static_cast<Opcode>(instruction & 0xFF);
    uint32_t operand = instruction >> 8;
    switch (opcode) {
      case Opcode::NO_OP:
        break;
      case Opcode::NIL: {
        stack.push_back(Value::NIL);
        break;
      }
      case Opcode::TRUE: {
        stack.push_back(Value::TRUE);
        break;
      }
      case Opcode::FALSE: {
        stack.push_back(Value::FALSE);
        break;
      }
      case Opcode::CONST: {
        stack.push_back(chunk.constants[operand]);
        break;
      }
      case Opcode::ADD: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() + b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() + b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::SUB: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() - b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() - b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::MUL: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() * b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() * b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::DIV: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() / b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() / b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::MOD: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() % b.toInt());
        break;
      }
      case Opcode::NEG: {
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(-a.toInt());
            break;
          case 2:
            stack.push_back(-a.toDouble());
            break;
        }
        break;
      }
      case Opcode::EQ: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a == b);
        break;
      }
      case Opcode::NEQ: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a != b);
      }
      case Opcode::LT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() < b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() < b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::LTE: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() <= b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() <= b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::GT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() > b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() > b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::GTE: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.toInt() >= b.toInt());
            break;
          case 2:
            stack.push_back(a.toDouble() >= b.toDouble());
            break;
          default:
            throw std::runtime_error("Unknown operand");
        }
        break;
      }
      case Opcode::AND: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toBool() && b.toBool());
        break;
      }
      case Opcode::OR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toBool() || b.toBool());
        break;
      }
      case Opcode::NOT: {
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(!a.toBool());
        break;
      }
      case Opcode::BIT_AND: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() & b.toInt());
        break;
      }
      case Opcode::BIT_OR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() | b.toInt());
        break;
      }
      case Opcode::BIT_XOR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() ^ b.toInt());
        break;
      }
      case Opcode::BIT_NOT: {
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(~a.toInt());
        break;
      }
      case Opcode::SHIFT_LEFT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() << b.toInt());
        break;
      }
      case Opcode::SHIFT_RIGHT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() >> b.toInt());
        break;
      }
      case Opcode::LOAD: {
        int stackSlot = bp + operand;
        stack.push_back(stack[stackSlot]);
        break;
      }
      case Opcode::STORE: {
        int stackSlot = bp + operand;
        stack[stackSlot] = stack.back();
        stack.pop_back();
        break;
      }
      case Opcode::DUP: {
        stack.push_back(stack.back());
        break;
      }
      case Opcode::POP: {
        stack.pop_back();
        break;
      }
      case Opcode::TEST: {
        Value condition = stack.back();
        stack.pop_back();
        if (condition.toBool()) {
          ip++;
        }
        break;
      }
      case Opcode::JUMP: {
        ip = operand;
        break;
      }
      case Opcode::RETURN: {
        return stack.back();
      }
      default:
        throw std::runtime_error("Unimplemented opcode");
    }
  }
}
