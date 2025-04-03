#include "vm.h"

#include <iostream>
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
      case Opcode::CONST: {
        stack.push_back(chunk.constants[operand]);
        break;
      }
      case Opcode::LOAD: {
        int stackSlot = bp + operand;
        stack.push_back(stack[stackSlot]);
        break;
      }
      case Opcode::ADD: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() + b.toInt());
        break;
      }
      case Opcode::SUB: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.toInt() - b.toInt());
        break;
      }
      case Opcode::RETURN: {
        if (callStack.empty()) {
          return stack.back();
        }
      }
      default:
        throw std::runtime_error("Unimplemented opcode");
    }
  }
}
