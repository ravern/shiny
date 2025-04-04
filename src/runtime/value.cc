#include "value.h"

#include <cstdint>
#include <utility>

const Value Value::NIL = Value((uint64_t)0);
const Value Value::TRUE = Value((uint64_t)1);
const Value Value::FALSE = Value((uint64_t)2);

Value::Value(uint64_t raw) : raw(raw) {}
Value::Value(bool b) { b ? *this = TRUE : *this = FALSE; }
Value::Value(int64_t i) : raw(reinterpret_cast<uint64_t &>(i)) {}
Value::Value(double d) : raw(reinterpret_cast<uint64_t &>(d)) {}

template <typename T>
Value::Value(ObjectPtr<T> &&o)
    : raw(reinterpret_cast<uint64_t &>(std::move(o))) {}

uint64_t Value::toRaw() const { return raw; }
bool Value::toBool() const { return *this == TRUE; }
int64_t Value::toInt() const { return reinterpret_cast<const int64_t &>(raw); }
double Value::toDouble() const { return reinterpret_cast<const double &>(raw); }

template <typename T>
ObjectPtr<T> &Value::toObject() {
  return reinterpret_cast<ObjectPtr<T> &>(raw);
}

bool Value::operator==(const Value &other) const { return raw == other.raw; }
bool Value::operator!=(const Value &other) const { return raw != other.raw; }