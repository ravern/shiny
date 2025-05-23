#pragma once

#include <iostream>
#include <vector>

#include "debug.h"
#include "frontend/type.h"
#include "runtime/object.h"
#include "runtime/object_ptr.h"

static Value print(std::vector<Value>& args, StringInterner& stringInterner) {
  std::cout << valueToString(args[0], stringInterner) << std::endl;
  return Value::NIL;
}

struct BuiltInEntry {
  std::string name;
  std::shared_ptr<Type> type;
  ObjectPtr<BuiltInObject> object;
};

static const std::vector<BuiltInEntry> builtIns = {
    {"__built_in__print__",
     std::make_shared<FunctionType>(std::vector<std::shared_ptr<Type>>{},
                                    std::make_shared<VoidType>()),
     ObjectPtr<BuiltInObject>(
         std::move(BuiltInObject(print, "__built_in__print__")))},
};
