#pragma once

#include <iostream>

#include "debug.h"
#include "frontend/type.h"
#include "runtime/object.h"

static Value print(std::vector<Value>& args, StringInterner& stringInterner) {
  std::cout << valueToString(args[0], stringInterner) << std::endl;
  return Value::NIL;
}

static Value arrayAppend(std::vector<Value>& args,
                         StringInterner& stringInterner) {
  auto array = args[0].asObject<ArrayObject>();
  array->append(args[1]);
  return Value::NIL;
}

struct BuiltInEntry {
  std::string name;
  std::shared_ptr<Type> type;
  BuiltInObject builtIn;
};

static const BuiltInEntry builtIns[] = {
    {"__built_in__print__",
     std::make_shared<FunctionType>(std::vector<std::shared_ptr<Type>>{},
                                    std::make_shared<Type>(TypeKind::Void)),
     BuiltInObject(print, "__built_in__print__")},
    {"__built_in__array_append__",
     std::make_shared<FunctionType>(
         std::vector<std::shared_ptr<Type>>{},
         std::make_shared<Type>(TypeKind::Void)),  // FIXME
     BuiltInObject(arrayAppend, "__built_in__array_append__")},
};
