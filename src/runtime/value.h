#pragma once

#include <bit>
#include <stdexcept>
#include <utility>

template <typename T>
class ObjectPtr;

class Value {
 public:
  static const Value NIL;
  static const Value TRUE;
  static const Value FALSE;

 public:
  Value() : raw(0) {}
  Value(uint64_t raw) : raw(raw) {}
  Value(double d) : raw(std::bit_cast<uint64_t>(d)) {}

  Value(bool b) {
    if (b) {
      *this = TRUE;
    } else {
      *this = FALSE;
    }
  }

  Value(int64_t i) {
    if (i > MAX_INT || i < MIN_INT) {
      throw std::runtime_error(
          "Value is out of range, only up to 48-bit integers are supported");
    }
    raw = MASK_NAN | (MASK_PAYLOAD & i << NUM_TAG_BITS) | TAG_INT;
  }

  Value(const Value& other);
  Value(Value&& other);

  template <typename T>
  Value(ObjectPtr<T>&& o) {
    initObject(std::move(o));
  }

  template <typename T>
  Value(const ObjectPtr<T>& o) {
    ObjectPtr<T> ptr = o;
    initObject(std::move(ptr));
  }

  ~Value();

  bool isNil() const { return *this == NIL; }
  bool isBool() const { return *this == TRUE || *this == FALSE; }
  bool isInt() const { return !isDouble() && (raw & MASK_TAG) == TAG_INT; }
  bool isDouble() const { return (raw & MASK_NAN) != MASK_NAN; }

  bool isAnyObject() const {
    return !isDouble() && (raw & MASK_TAG) == TAG_OBJ;
  }

  template <typename T>
  bool isObject() const {
    return isAnyObject() && ObjectPtr<T>::__is(getPayload());
  }

  bool asBool() const { return *this == TRUE; }

  int64_t asInt() const {
    uint64_t payload = getPayload();
    // make sure to sign-extend a signed int
    if ((payload & MASK_INT_SIGN) == MASK_INT_SIGN) {
      return std::bit_cast<int64_t>(payload | MASK_INT_SIGN_EXTEND);
    } else {
      return std::bit_cast<int64_t>(payload);
    }
  }

  double asDouble() const { return std::bit_cast<double>(raw); }

  template <typename T>
  const ObjectPtr<T> asObject() const {
    return ObjectPtr<T>(getPayload());
  }

  template <typename T>
  ObjectPtr<T> asObject() {
    return ObjectPtr<T>(getPayload());
  }

  uint64_t __getRaw() const { return raw; }

  Value& operator=(const Value& other);
  Value& operator=(Value&& other);

  bool operator==(const Value& other) const { return raw == other.raw; }
  bool operator!=(const Value& other) const { return raw != other.raw; }

 private:
  static constexpr const uint64_t MASK_NAN = 0x7FF8000000000000;
  static constexpr const uint64_t MASK_PAYLOAD = 0x0007FFFFFFFFFFFF;
  static constexpr const uint64_t MASK_TAG = 0x0000000000000007;
  static constexpr const uint64_t MASK_INT_SIGN = 0x0000800000000000;
  static constexpr const uint64_t MASK_INT_SIGN_EXTEND = 0xFFFF000000000000;
  static constexpr const int NUM_TAG_BITS = 3;
  static constexpr const uint64_t TAG_NIL = 0x0000000000000000;
  static constexpr const uint64_t TAG_TRUE = 0x0000000000000001;
  static constexpr const uint64_t TAG_FALSE = 0x0000000000000002;
  static constexpr const uint64_t TAG_INT = 0x0000000000000003;
  static constexpr const uint64_t TAG_OBJ = 0x0000000000000004;
  static constexpr const int64_t MAX_INT = 0x00007FFFFFFFFFFF;
  static constexpr const int64_t MIN_INT = -0x0000800000000000;

  template <typename T>
  void initObject(ObjectPtr<T>&& o) {
    uint64_t payload = o.__forget() << NUM_TAG_BITS | TAG_OBJ;
    if ((MASK_PAYLOAD & payload) != payload) {
      throw std::runtime_error(
          "Object pointer occupies more than 48-bits, this is not supported");
    }
    raw = MASK_NAN | payload;
  }

  uint64_t getPayload() const { return (raw & MASK_PAYLOAD) >> NUM_TAG_BITS; }

  uint64_t raw;
};
