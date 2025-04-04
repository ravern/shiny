#include "object_ptr.h"

#include <variant>

template <typename T>
ObjectPtr<T>::ObjectPtr() : ptr(nullptr) {}

template <typename T>
ObjectPtr<T>::ObjectPtr(T&& o) : ptr(new Object(std::move(o))) {}

template <>
ObjectPtr<std::monostate>::ObjectPtr(std::monostate&& o) : ptr(nullptr) {}

template <typename T>
ObjectPtr<T>::ObjectPtr(uint64_t raw) : ptr(std::bit_cast<Object*>(raw)) {
  ptr->strongCount++;
}

template <typename T>
ObjectPtr<T>::ObjectPtr(const ObjectPtr& other) : ptr(other.ptr) {
  ptr->strongCount++;
}

template <typename T>
ObjectPtr<T>::ObjectPtr(ObjectPtr&& other) : ptr(other.ptr) {
  other.ptr = nullptr;
}

template <typename T>
ObjectPtr<T>::~ObjectPtr() noexcept(false) {
  if (ptr == nullptr) {
    return;
  }
  if (ptr->strongCount == 0) {
    throw std::runtime_error("Tried to decrement strong count of 0");
  }
  ptr->strongCount--;
  if (ptr->strongCount == 0 && ptr->weakCount == 0) {
    delete ptr;
  }
}

template <typename T>
ObjectPtr<T>& ObjectPtr<T>::operator=(const ObjectPtr& other) {
  ptr = other.ptr;
  if (ptr != nullptr) {
    ptr->strongCount++;
  }
  return *this;
}

template <typename T>
ObjectPtr<T>& ObjectPtr<T>::operator=(ObjectPtr&& other) {
  ptr = other.ptr;
  other.ptr = nullptr;
  return *this;
}

template <typename T>
T* ObjectPtr<T>::operator->() {
  return ptr->get<T>();
}

template <>
std::monostate* ObjectPtr<std::monostate>::operator->() {
  return nullptr;
}

template <typename T>
ObjectPtr<T> ObjectPtr<T>::remember(uint64_t raw) {
  ObjectPtr<T> ptr;
  ptr.ptr = std::bit_cast<Object*>(raw);
  return ptr;
}

template <typename T>
uint64_t ObjectPtr<T>::forget() {
  uint64_t raw = std::bit_cast<uint64_t>(ptr);
  ptr = nullptr;
  return raw;
}

template class ObjectPtr<FunctionObject>;
template class ObjectPtr<UpvalueObject>;
template class ObjectPtr<ClosureObject>;
template class ObjectPtr<std::monostate>;
