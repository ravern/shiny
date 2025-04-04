#include "value.h"

#include <variant>

#include "object_ptr.h"

const Value Value::NIL(uint64_t(MASK_NAN | TAG_NIL));
const Value Value::TRUE(uint64_t(MASK_NAN | TAG_TRUE));
const Value Value::FALSE(uint64_t(MASK_NAN | TAG_FALSE));

Value::~Value() {
  if (isObject()) {
    ObjectPtr<std::monostate>::remember((raw & MASK_PAYLOAD) >> NUM_TAG_BITS);
  }
}

Value::Value(const Value& other) {
  if (other.isObject()) {
    ObjectPtr<std::monostate> ptr = other.asObject<std::monostate>();
    initObject<std::monostate>(std::move(ptr));
  } else {
    raw = other.raw;
  }
}

Value::Value(Value&& other) {
  raw = other.raw;
  other = Value::NIL;
}

Value& Value::operator=(const Value& other) {
  if (other.isObject()) {
    ObjectPtr<std::monostate> ptr = other.asObject<std::monostate>();
    initObject<std::monostate>(std::move(ptr));
  } else {
    raw = other.raw;
  }
  return *this;
}

Value& Value::operator=(Value&& other) {
  raw = other.raw;
  other.raw = 0;
  return *this;
}
