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
  chunk = &closure->getFunction()->getChunk();

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
      case Opcode::ARRAY: {
        stack.push_back(Value(std::move(ObjectPtr<ArrayObject>())));
        break;
      }
      case Opcode::DICT: {
        stack.push_back(Value(std::move(ObjectPtr<DictObject>())));
        break;
      }
      case Opcode::CLOSURE: {
        ObjectPtr<FunctionObject> newFunction =
            chunk->constants[operand].asObject<FunctionObject>();

        // Capture all the upvalues to create the closure
        std::vector<ObjectPtr<UpvalueObject>> upvalues;
        for (auto& functionUpvalue : newFunction->getUpvalues()) {
          if (functionUpvalue.isLocal) {
            int stackSlot = bp + functionUpvalue.index;
            auto upvalue = pushUpvalue(&stack[stackSlot]);
            upvalues.push_back(upvalue);
          } else {
            auto parentClosure = callStack.back().closure;
            auto upvalue = parentClosure->getUpvalue(functionUpvalue.index);
            upvalues.push_back(upvalue);
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
      case Opcode::GLOBAL_LOAD: {
        stack.push_back(globals[operand]);
        break;
      }
      case Opcode::GLOBAL_STORE: {
        if (operand >= globals.size()) {
          globals.resize(operand + 1); // Resize to allow new globals
        }
        globals[operand] = stack.back();
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
        pushFrame(operand);
        break;
      }
      case Opcode::RETURN: {
        if (callStack.empty()) {
          throw std::runtime_error(
              "Tried to return from the top-level function");
        }

        // Close all the upvalues up to stack base
        closeUpvalues(&stack[bp]);

        // Pop everything up to the base pointer
        Value returnValue = stack.back();
        while (stack.size() > bp) {
          stack.pop_back();
        }
        stack.push_back(returnValue);

        popFrame();
        break;
      }
      case Opcode::HALT: {
        std::cout << "HALTING WITH RESULT " << stack.back().asInt()
                  << std::endl;
        return;
      }

      // Opcodes for upvalue manipulation
      case Opcode::UPVALUE_LOAD: {
        int upvalueIndex = operand;
        auto upvalue = closure->getUpvalue(upvalueIndex);
        stack.push_back(*upvalue->getValue());
        break;
      }
      case Opcode::UPVALUE_STORE: {
        int upvalueIndex = operand;
        auto upvalue = closure->getUpvalue(upvalueIndex);
        *upvalue->getValue() = stack.back();
        stack.pop_back();
        break;
      }
      case Opcode::UPVALUE_CLOSE: {
        closeUpvalues(&stack[stack.size() - 1]);
        break;
      }

      default:
        throw std::runtime_error("Unimplemented opcode");
    }
  }
}

void VM::pushFrame(int arity) {
  int newBp = stack.size() - arity - 1;
  auto newClosure = stack[newBp].asObject<ClosureObject>();
  callStack.push_back({closure, ip, bp});
  closure = newClosure;
  ip = 0;
  bp = newBp;
  chunk = &closure->getFunction()->getChunk();
}

void VM::popFrame() {
  Frame frame = callStack.back();
  callStack.pop_back();
  closure = frame.closure;
  ip = frame.ip;
  bp = frame.bp;
  chunk = &closure->getFunction()->getChunk();
}

ObjectPtr<UpvalueObject> VM::captureUpvalue(Upvalue functionUpvalue) {
  if (functionUpvalue.isLocal) {
    int stackSlot = bp + functionUpvalue.index;
    auto upvalue = pushUpvalue(&stack[stackSlot]);
    return upvalue;
  } else {
    auto parentClosure = callStack.back().closure;
    auto upvalue = parentClosure->getUpvalue(functionUpvalue.index);
    return upvalue;
  }
}

ObjectPtr<UpvalueObject> VM::pushUpvalue(Value* value) {
  ObjectPtr<UpvalueObject> upvalue;
  if (upvalueStack.has_value()) {
    upvalue = ObjectPtr<UpvalueObject>(
        std::move(UpvalueObject(value, upvalueStack.value())));
  } else {
    upvalue = ObjectPtr<UpvalueObject>(UpvalueObject(value));
  }
  upvalueStack = upvalue;
  return upvalue;
}

void VM::closeUpvalues(Value* upTillValue) {
  std::optional<ObjectPtr<UpvalueObject>> prev = std::nullopt;
  std::optional<ObjectPtr<UpvalueObject>> current = upvalueStack;
  while (true) {
    if (!current.has_value() ||
        current.value().get()->getValue() < upTillValue) {
      break;
    }
    current.value()->close();
    if (prev.has_value()) {
      prev.value()->getNext() = current.value()->getNext();
    } else {
      upvalueStack = current.value()->getNext();
    }
    prev = current;
    current = current.value()->getNext();
  }
}
