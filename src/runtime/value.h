#pragma once

#include <cstdint>

#include "object_ref.h"

enum class ValueType {
  Int,
  Double,
  Object,
};

class Value {
 public:
  Value(int64_t i);
  Value(double d);
  Value(ObjectRef obj);

 public:
  int64_t toInt() const;
  double toDouble() const;
  ObjectRef toObject();

 private:
  uint64_t raw;
};