#include "value.h"

Value::Value(int64_t i) : raw(reinterpret_cast<uint64_t&>(i)) {}
Value::Value(double d) : raw(reinterpret_cast<uint64_t&>(d)) {}
Value::Value(ObjectRef obj) : raw(reinterpret_cast<uint64_t&>(obj)) {}

int64_t Value::toInt() const { return reinterpret_cast<const int64_t&>(raw); }
double Value::toDouble() const { return reinterpret_cast<const double&>(raw); }
ObjectRef Value::toObject() { return reinterpret_cast<const ObjectRef&>(raw); }
