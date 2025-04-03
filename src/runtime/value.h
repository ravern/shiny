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
  static const Value NIL;
  static const Value TRUE;
  static const Value FALSE;

 public:
  Value(uint64_t raw);
  Value(bool b);
  Value(int64_t i);
  Value(double d);
  Value(ObjectRef o);

 public:
  bool operator==(const Value& other) const;
  bool operator!=(const Value& other) const;

 public:
  bool toBool() const;
  int64_t toInt() const;
  double toDouble() const;
  ObjectRef toObject();

 private:
  uint64_t raw;
};
