#include "vm.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../debug.h"
#include "../runtime/object.h"
#include "../runtime/object_ptr.h"
#include "../runtime/value.h"

VM::VM() {}

void VM::evaluate(ObjectPtr<FunctionObject> function) {
  // Initialize the VM state for a new evaluation
  closure = ObjectPtr<ClosureObject>(ClosureObject(std::move(function)));
  ip = 0;
  bp = 0;
  Chunk* chunk = &closure->getFunction()->getChunk();

  while (true) {
    // Fetch and decode the current instruction
    Instruction instruction = chunk->instructions[ip++];
    Opcode opcode = static_cast<Opcode>(instruction & 0xFF);
    uint32_t operand = instruction >> 8;

    std::cout << "ip: " << ip << " opcode: " << opcodeToString(opcode)
              << " operand: " << operand << std::endl;

    // Execute the instruction
    switch (opcode) {
      case Opcode::NO_OP:
        break;

      // Opcodes that push new values onto the stack
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
        stack.push_back(chunk->constants[operand]);
        break;
      }
      case Opcode::CLOSURE: {
        ObjectPtr<FunctionObject> newFunction =
            chunk->constants[operand].asObject<FunctionObject>();

        // Capture all the upvalues to create the closure
        std::vector<ObjectPtr<UpvalueObject>> upvalues;
        for (auto& upvalue : newFunction->getUpvalues()) {
          if (upvalue.isLocal) {
            int stackSlot = bp + upvalue.index;
            ObjectPtr<UpvalueObject> upvalueObject(
                std::move(UpvalueObject(&stack[stackSlot])));
            upvalueStack.push_back(upvalueObject);
            upvalues.push_back(upvalueObject);
          } else {
            ObjectPtr<ClosureObject> parentClosure = callStack.back().closure;
            ObjectPtr<UpvalueObject> parentUpvalue =
                parentClosure->getUpvalues()[upvalue.index];
            upvalues.push_back(parentUpvalue);
          }
        }

        // FIXME: what a monstrosity
        stack.push_back(Value(std::move(ObjectPtr<ClosureObject>(
            std::move(ClosureObject(newFunction, std::move(upvalues)))))));

        break;
      }

      // Opcodes to perform arithmetic
      case Opcode::ADD: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(a.asInt() + b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() + b.asDouble());
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
            stack.push_back(a.asInt() - b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() - b.asDouble());
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
            stack.push_back(a.asInt() * b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() * b.asDouble());
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
            stack.push_back(a.asInt() / b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() / b.asDouble());
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
        stack.push_back(a.asInt() % b.asInt());
        break;
      }
      case Opcode::NEG: {
        Value a = stack.back();
        stack.pop_back();
        switch (operand) {
          case 1:
            stack.push_back(-a.asInt());
            break;
          case 2:
            stack.push_back(-a.asDouble());
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
            stack.push_back(a.asInt() < b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() < b.asDouble());
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
            stack.push_back(a.asInt() <= b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() <= b.asDouble());
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
            stack.push_back(a.asInt() > b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() > b.asDouble());
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
            stack.push_back(a.asInt() >= b.asInt());
            break;
          case 2:
            stack.push_back(a.asDouble() >= b.asDouble());
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
        stack.push_back(a.asBool() && b.asBool());
        break;
      }
      case Opcode::OR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.asBool() || b.asBool());
        break;
      }
      case Opcode::NOT: {
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(!a.asBool());
        break;
      }
      case Opcode::BIT_AND: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.asInt() & b.asInt());
        break;
      }
      case Opcode::BIT_OR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.asInt() | b.asInt());
        break;
      }
      case Opcode::BIT_XOR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.asInt() ^ b.asInt());
        break;
      }
      case Opcode::BIT_NOT: {
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(~a.asInt());
        break;
      }
      case Opcode::SHIFT_LEFT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.asInt() << b.asInt());
        break;
      }
      case Opcode::SHIFT_RIGHT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        stack.push_back(a.asInt() >> b.asInt());
        break;
      }

      // Opcodes for stack manipulation
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

      // Opcodes for control flow
      case Opcode::TEST: {
        Value condition = stack.back();
        stack.pop_back();
        if (condition.asBool()) {
          ip++;
        }
        break;
      }
      case Opcode::JUMP: {
        ip = operand;
        break;
      }
      case Opcode::CALL: {
        callStack.push_back({closure, ip, bp});
        int functionStackSlot = stack.size() - operand - 1;
        ObjectPtr<ClosureObject> newClosure =
            stack[functionStackSlot].asObject<ClosureObject>();
        closure = newClosure;
        ip = 0;
        bp = functionStackSlot;
        chunk = &closure->getFunction()->getChunk();
        break;
      }
      case Opcode::RETURN: {
        if (callStack.empty()) {
          throw std::runtime_error(
              "Tried to return from the top-level function");
        }

        // Pop everything up to the base pointer
        Value returnValue = stack.back();
        while (stack.size() > bp) {
          stack.pop_back();
        }
        stack.push_back(returnValue);

        // Restore the frame
        Frame frame = callStack.back();
        callStack.pop_back();
        closure = frame.closure;
        ip = frame.ip;
        bp = frame.bp;
        chunk = &closure->getFunction()->getChunk();
        break;
      }
      case Opcode::HALT: {
        std::cout << stack.back().asInt() << std::endl;
        return;
      }
      default:
        throw std::runtime_error("Unimplemented opcode");
    }
  }
}
