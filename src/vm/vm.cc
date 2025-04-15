#include "vm.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../debug.h"
#include "../runtime/object.h"
#include "../runtime/object_ptr.h"
#include "../runtime/value.h"

VM::VM(StringInterner& stringInterner) : stringInterner(stringInterner) {}
VM::VM(StringInterner& stringInterner, std::vector<Value>&& globals)
    : stringInterner(stringInterner), globals(std::move(globals)) {}

Value VM::evaluate(ObjectPtr<FunctionObject> function) {
  std::cout << "==== Starting evaluation ====" << std::endl;

  // Initialize the VM state for a new evaluation
  currentFunction = ObjectPtr<ClosureObject>(std::move(function));
  ip = 0;
  bp = 0;
  chunk = &getFunctionFromValue(currentFunction)->getChunk();
  lastPoppedValue = Value::NIL;

  while (true) {
    // Fetch and decode the current instruction
    Instruction instruction = chunk->instructions[ip++];
    Opcode opcode = static_cast<Opcode>(instruction & 0xFF);
    uint32_t operand = instruction >> 8;

    std::cout << instructionToString(ip - 1, instruction, stringInterner)
              << std::endl;

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
      case Opcode::CLASS: {
        ObjectPtr<ClassObject> newClass =
            chunk->constants[operand].asObject<ClassObject>();
        stack.push_back(Value(std::move(newClass)));
        break;
      }
      case Opcode::CLOSURE: {
        ObjectPtr<FunctionObject> newFunction =
            chunk->constants[operand].asObject<FunctionObject>();

        // Capture all the upvalues to create the closure
        std::vector<ObjectPtr<UpvalueObject>> upvalues;
        for (auto& functionUpvalue : newFunction->getUpvalues()) {
          upvalues.push_back(captureUpvalue(functionUpvalue));
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
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() + b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() + b.asDouble());
        } else if (a.isObject<StringObject>() && b.isObject<StringObject>()) {
          stack.push_back(Value(std::move(ObjectPtr<StringObject>(std::move(
              StringObject(a.asObject<StringObject>()->getData() +
                           b.asObject<StringObject>()->getData()))))));
        } else {
          throw std::runtime_error("Invalid operand types for add");
        }
        break;
      }
      case Opcode::SUB: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() - b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() - b.asDouble());
        } else {
          throw std::runtime_error("Invalid operand types for sub");
        }
        break;
      }
      case Opcode::MUL: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() * b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() * b.asDouble());
        } else {
          throw std::runtime_error("Invalid operand types for mul");
        }
        break;
      }
      case Opcode::DIV: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() / b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() / b.asDouble());
        } else {
          throw std::runtime_error("Invalid operand types for div");
        }
        break;
      }
      case Opcode::MOD: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for mod");
        }
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
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for equal");
        }
        stack.push_back(a == b);
        break;
      }
      case Opcode::NEQ: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for not-equal");
        }
        stack.push_back(a != b);
      }
      case Opcode::LT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() < b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() < b.asDouble());
        } else {
          throw std::runtime_error("Invalid operand types for less-than");
        }
        break;
      }
      case Opcode::LTE: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() <= b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() <= b.asDouble());
        } else {
          throw std::runtime_error(
              "Invalid operand types for less-than-or-equal");
        }
        break;
      }
      case Opcode::GT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() > b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() > b.asDouble());
        } else {
          throw std::runtime_error("Invalid operand types for greater-than");
        }
        break;
      }
      case Opcode::GTE: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (a.isInt() && b.isInt()) {
          stack.push_back(a.asInt() >= b.asInt());
        } else if (a.isDouble() && b.isDouble()) {
          stack.push_back(a.asDouble() >= b.asDouble());
        } else {
          throw std::runtime_error(
              "Invalid operand types for greater-than-or-equal");
        }
        break;
      }
      case Opcode::AND: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isBool() || !b.isBool()) {
          throw std::runtime_error("Invalid operand types for and");
        }
        stack.push_back(a.asBool() && b.asBool());
        break;
      }
      case Opcode::OR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isBool() || !b.isBool()) {
          throw std::runtime_error("Invalid operand types for or");
        }
        stack.push_back(a.asBool() || b.asBool());
        break;
      }
      case Opcode::NOT: {
        Value a = stack.back();
        stack.pop_back();
        if (!a.isBool()) {
          throw std::runtime_error("Invalid operand types for not");
        }
        stack.push_back(!a.asBool());
        break;
      }
      case Opcode::BIT_AND: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for bit-and");
        }
        stack.push_back(a.asInt() & b.asInt());
        break;
      }
      case Opcode::BIT_OR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for bit-or");
        }
        stack.push_back(a.asInt() | b.asInt());
        break;
      }
      case Opcode::BIT_XOR: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for bit-xor");
        }
        stack.push_back(a.asInt() ^ b.asInt());
        break;
      }
      case Opcode::BIT_NOT: {
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt()) {
          throw std::runtime_error("Invalid operand types for bit-not");
        }
        stack.push_back(~a.asInt());
        break;
      }
      case Opcode::SHIFT_LEFT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for shift-left");
        }
        stack.push_back(a.asInt() << b.asInt());
        break;
      }
      case Opcode::SHIFT_RIGHT: {
        Value b = stack.back();
        stack.pop_back();
        Value a = stack.back();
        stack.pop_back();
        if (!a.isInt() || !b.isInt()) {
          throw std::runtime_error("Invalid operand types for shift-right");
        }
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
        lastPoppedValue = stack.back();
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
        if (operand == 0 && stack.back().isObject<ClassObject>()) {
          callClass();
          break;
        }

        pushFrame(operand);

        if (currentFunction.isObject<MethodObject>()) {
          stack[bp] = currentFunction.asObject<MethodObject>()->getSelf();
        }

        printStack();

        // Print entering frame
        std::optional<SymbolId> name =
            getFunctionFromValue(callStack.back().function)->getName();
        std::cout << "== Entering "
                  << (name.has_value() ? stringInterner.get(name.value())
                                       : "<anonymous>")
                  << " ==" << std::endl;

        // Skip printing the stack at the end of iteration (already printed the
        // stack above)
        continue;
      }
      case Opcode::RETURN: {
        if (callStack.empty()) {
          throw std::runtime_error(
              "Tried to return from the top-level function");
        }

        // Close all the upvalues up to stack base
        closeUpvalues(bp);

        // Pop everything up to the base pointer
        Value returnValue = stack.back();
        while (stack.size() > bp) {
          stack.pop_back();
        }
        stack.push_back(returnValue);

        // Print the stack here
        printStack();

        // Print leaving frame
        std::optional<SymbolId> name =
            getFunctionFromValue(callStack.back().function)->getName();
        std::cout << "== Leaving "
                  << (name.has_value() ? stringInterner.get(name.value())
                                       : "<anonymous>")
                  << " ==" << std::endl;

        popFrame();

        // Skip printing the stack at the end of iteration (already printed the
        // stack above)
        continue;
      }
      case Opcode::HALT: {
        std::cout << "==== Evaluation complete ====" << std::endl;
        return lastPoppedValue;
      }

      // Opcodes for globals
      case Opcode::GLOBAL_LOAD: {
        stack.push_back(globals[operand]);
        break;
      }
      case Opcode::GLOBAL_STORE: {
        if (operand >= globals.size()) {
          globals.resize(operand + 1);  // Resize to allow new globals
        }
        globals[operand] = stack.back();
        stack.pop_back();
        break;
      }

      // Opcodes for upvalue manipulation
      case Opcode::UPVALUE_LOAD: {
        int upvalueIndex = operand;
        auto upvalue =
            currentFunction.asObject<ClosureObject>()->getUpvalue(upvalueIndex);
        stack.push_back(upvalue->getValue(stack));
        break;
      }
      case Opcode::UPVALUE_STORE: {
        int upvalueIndex = operand;
        auto upvalue =
            currentFunction.asObject<ClosureObject>()->getUpvalue(upvalueIndex);
        upvalue->setValue(stack.back(), stack);
        stack.pop_back();
        break;
      }
      case Opcode::UPVALUE_CLOSE: {
        closeUpvalues(stack.size() - 1);
        break;
      }

      // Opcodes for arrays
      case Opcode::ARRAY_GET: {
        int index = stack.back().asInt();
        stack.pop_back();
        auto array = stack.back().asObject<ArrayObject>();
        stack.pop_back();
        stack.push_back(array->get(index));
        break;
      }
      case Opcode::ARRAY_SET: {
        Value value = stack.back();
        stack.pop_back();
        int index = stack.back().asInt();
        stack.pop_back();
        auto array = stack.back().asObject<ArrayObject>();
        stack.pop_back();
        array->set(index, value);
        break;
      }

      // Opcodes for dictionaries
      case Opcode::DICT_GET: {
        Value key = stack.back();
        stack.pop_back();
        auto dict = stack.back().asObject<DictObject>();
        stack.pop_back();
        stack.push_back(dict->get(key));
        break;
      }
      case Opcode::DICT_SET: {
        Value value = stack.back();
        stack.pop_back();
        Value key = stack.back();
        stack.pop_back();
        auto dict = stack.back().asObject<DictObject>();
        stack.pop_back();
        dict->set(key, value);
        break;
      }

      // Opcodes for instances
      case Opcode::MEMBER_GET: {
        auto instance = stack.back().asObject<InstanceObject>();
        stack.push_back(instance->getMember(operand));
        break;
      }
      case Opcode::MEMBER_SET: {
        auto instance = stack.back().asObject<InstanceObject>();
        instance->setMember(operand, stack.back());
        stack.pop_back();
        break;
      }

      default:
        throw std::runtime_error("Unimplemented opcode");
    }

    printStack();
  }
}

void VM::callClass() {
  auto klass = stack.back().asObject<ClassObject>();
  ObjectPtr<InstanceObject> instance(std::move(klass));
  for (int i = 0; i < klass->getMembers().size(); i++) {
    auto member = klass->getMembers()[i];
    if (member.isObject<FunctionObject>()) {
      instance->setMember(
          i, Value(std::move(ObjectPtr<MethodObject>(std::move(
                 MethodObject(std::move(member.asObject<FunctionObject>()),
                              std::move(instance)))))));
    }
  }
  stack.pop_back();
  stack.push_back(Value(std::move(instance)));
}

void VM::pushFrame(int arity) {
  int newBp = stack.size() - arity - 1;
  Value newFunction = stack[newBp];
  callStack.push_back({currentFunction, ip, bp});
  currentFunction = newFunction;
  ip = 0;
  bp = newBp;
  chunk = &getFunctionFromValue(currentFunction)->getChunk();
}

void VM::popFrame() {
  Frame frame = callStack.back();
  callStack.pop_back();
  currentFunction = frame.function;
  ip = frame.ip;
  bp = frame.bp;
  chunk = &getFunctionFromValue(currentFunction)->getChunk();
}

ObjectPtr<UpvalueObject> VM::captureUpvalue(Upvalue functionUpvalue) {
  if (functionUpvalue.isLocal) {
    int stackSlot = bp + functionUpvalue.index;
    auto upvalue = pushUpvalue(stackSlot);
    return upvalue;
  } else {
    auto parentClosure = callStack.back().function.asObject<ClosureObject>();
    auto upvalue = parentClosure->getUpvalue(functionUpvalue.index);
    return upvalue;
  }
}

ObjectPtr<UpvalueObject> VM::pushUpvalue(int stackSlot) {
  ObjectPtr<UpvalueObject> upvalue(std::move(UpvalueObject(stackSlot)));
  if (upvalueStack.has_value()) {
    upvalue = ObjectPtr<UpvalueObject>(
        std::move(UpvalueObject(stackSlot, upvalueStack.value())));
  } else {
    upvalue = ObjectPtr<UpvalueObject>(UpvalueObject(stackSlot));
  }
  upvalueStack = upvalue;
  return upvalue;
}

void VM::closeUpvalues(int upTillStackSlot) {
  std::optional<ObjectPtr<UpvalueObject>> prev = std::nullopt;
  std::optional<ObjectPtr<UpvalueObject>> current = upvalueStack;
  while (true) {
    if (!current.has_value() ||
        current.value()->getStackSlot() < upTillStackSlot) {
      break;
    }
    current.value()->close(stack);
    if (prev.has_value()) {
      prev.value()->getNext() = current.value()->getNext();
    } else {
      upvalueStack = current.value()->getNext();
    }
    prev = current;
    current = current.value()->getNext();
  }
}

void VM::printStack() {
  if (stack.empty()) {
    std::cout << "      <empty>" << std::endl;
  } else {
    std::cout << "      ";
    for (auto& value : stack) {
      std::cout << valueToString(value, stringInterner) << " ";
    }
    std::cout << std::endl;
  }
}

void VM::printUpvalueStack() {
  if (!upvalueStack.has_value()) {
    std::cout << "      <empty>" << std::endl;
  } else {
    std::cout << "      ";
    std::vector<ObjectPtr<UpvalueObject>> upvalues;
    auto current = upvalueStack;
    while (current.has_value()) {
      upvalues.insert(upvalues.begin(), current.value());
      current = current.value()->getNext();
    }
    for (auto& upvalue : upvalues) {
      std::cout << valueToString(Value(upvalue), stringInterner) << std::endl;
    }
  }
}

ObjectPtr<FunctionObject> VM::getFunctionFromValue(Value value) {
  if (value.isObject<ClosureObject>()) {
    return value.asObject<ClosureObject>()->getFunction();
  } else if (value.isObject<MethodObject>()) {
    return value.asObject<MethodObject>()->getFunction();
  } else {
    throw std::runtime_error(
        "Tried to access function from non-callable value");
  }
}