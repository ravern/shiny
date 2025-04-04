#include "vm.h"

#include <iostream>
#include <memory>
#include <stdexcept>

#include "../runtime/object.h"

Value VM::evaluate(ObjectRef function) {
  ObjectRef currentFunction = function;
  Chunk& chunk = currentFunction.toFunction().getChunk();
  int ip = 0;
  int bp = 0;
  std::vector<Value> stack;
  std::vector<Frame> callStack;
  std::vector<std::shared_ptr<LiveUpvalue>> upvalueStack;

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
        std::cout << "pushing constant " << chunk.constants[operand].toRaw()
                  << " to stack slot " << stack.size() << std::endl;
        stack.push_back(chunk.constants[operand]);
        break;
      }
      case Opcode::CLOSURE: {
        ObjectRef functionObject = chunk.constants[operand].toObject();
        FunctionObject& function = functionObject.toFunction();

        std::vector<std::shared_ptr<LiveUpvalue>> upvalues;
        for (auto& upvalue : function.getUpvalues()) {
          if (upvalue.isLocal) {
            int stackSlot = bp + upvalue.index;
            std::shared_ptr<LiveUpvalue> liveUpvalue =
                std::make_shared<LiveUpvalue>((uint64_t)0, &stack[stackSlot]);
            upvalueStack.push_back(liveUpvalue);
            upvalues.push_back(liveUpvalue);
          } else {
            ClosureObject& parentClosure =
                callStack.back().function.toClosure();
            std::shared_ptr<LiveUpvalue> parentUpvalue =
                parentClosure.getUpvalues()[upvalue.index];
            upvalues.push_back(parentUpvalue);
          }
        }

        std::cout << "pushing closure to stack slot " << stack.size()
                  << std::endl;
        stack.push_back(Value(
            ObjectRef(ClosureObject(functionObject, std::move(upvalues)))));
        std::cout << "stack size: " << stack.size() << std::endl;
        Value back = stack.back();
        std::cout << "is closure: " << back.toObject().isClosure()
                  << ", is function: " << back.toObject().isFunction()
                  << std::endl;

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
        std::cout << "loading value " << stack[stackSlot].toRaw()
                  << " from stack slot " << stackSlot << " to stack slot "
                  << stack.size() << std::endl;
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
      case Opcode::CALL: {
        callStack.push_back({currentFunction, ip, bp});
        std::cout << "operand: " << operand << std::endl;
        int functionStackSlot = stack.size() - operand - 1;
        std::cout << "stack size: " << stack.size() << std::endl;
        std::cout << "function stack slot: " << functionStackSlot << std::endl;
        std::cout << "top of stack: " << stack.back().toRaw() << std::endl;
        std::cout << "second top of stack: " << stack[stack.size() - 2].toRaw()
                  << std::endl;
        ObjectRef newFunction = stack[functionStackSlot].toObject();
        std::cout << "is closure: " << newFunction.isClosure()
                  << ", is function: " << newFunction.isFunction() << std::endl;
        currentFunction = newFunction;
        chunk = currentFunction.isFunction()
                    ? currentFunction.toFunction().getChunk()
                    : currentFunction.toClosure().getFunction().getChunk();
        bp = functionStackSlot;
        ip = 0;
        std::cout << "--------------------------------" << std::endl;
        break;
      }
      case Opcode::RETURN: {
        if (callStack.empty()) {
          return stack.back();
        }
        Frame frame = callStack.back();
        callStack.pop_back();
        currentFunction = frame.function;
        chunk = currentFunction.isFunction()
                    ? currentFunction.toFunction().getChunk()
                    : currentFunction.toClosure().getFunction().getChunk();
        ip = frame.ip;
        bp = frame.bp;
      }
      default:
        throw std::runtime_error("Unimplemented opcode");
    }
  }
}
