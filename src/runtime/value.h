#pragma once

#include <cstdint>

#include "object_ptr.h"

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
  template <typename T>
  Value(ObjectPtr<T>&& o);
  Value(const Value& other);
  Value(Value&& other);
  ~Value();

 public:
  Value& operator=(const Value& other);
  Value& operator=(Value&& other);
  bool operator==(const Value& other) const;
  bool operator!=(const Value& other) const;

 public:
  uint64_t toRaw() const;
  bool toBool() const;
  int64_t toInt() const;
  double toDouble() const;
  template <typename T>
  const ObjectPtr<T>& toObject() const;
  template <typename T>
  ObjectPtr<T>& toObject();

 private:
  uint64_t raw;
};

const Value Value::NIL = Value((uint64_t)0);
const Value Value::TRUE = Value((uint64_t)1);
const Value Value::FALSE = Value((uint64_t)0);

Value::Value(uint64_t raw) : raw(raw) {}
Value::Value(bool b) { b ? * this = TRUE : * this = FALSE; }
Value::Value(int64_t i) : raw(reinterpret_cast<uint64_t&>(i)) {}
Value::Value(double d) : raw(reinterpret_cast<uint64_t&>(d)) {}
