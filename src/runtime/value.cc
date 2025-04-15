#include "value.h"

#include <variant>

#include "object.h"
#include "object_ptr.h"

const Value Value::NIL(uint64_t(MASK_NAN | TAG_NIL));
const Value Value::TRUE(uint64_t(MASK_NAN | TAG_TRUE));
const Value Value::FALSE(uint64_t(MASK_NAN | TAG_FALSE));

Value::~Value() {
  if (isAnyObject()) {
    ObjectPtr<std::monostate>::__remember((raw & MASK_PAYLOAD) >> NUM_TAG_BITS);
  }
}

Value::Value(const Value& other) {
  if (other.isAnyObject()) {
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
  if (other.isAnyObject()) {
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

size_t std::hash<Value>::operator()(const Value& v) const {
  if (v.isObject<StringObject>()) {
    return std::hash<std::string>{}(v.asObject<StringObject>()->getData());
  }
  return std::hash<uint64_t>{}(v.__getRaw());
}