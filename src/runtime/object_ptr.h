#pragma once

#include <cstdint>

class Object;

template <typename T>
class ObjectPtr {
 public:
  ObjectPtr();
  ObjectPtr(T&& o);
  ObjectPtr(uint64_t raw);
  ObjectPtr(const ObjectPtr& other);
  ObjectPtr(ObjectPtr&& other);
  ~ObjectPtr() noexcept(false);

  T* get() const;

  ObjectPtr& operator=(const ObjectPtr& other);
  ObjectPtr& operator=(ObjectPtr&& other);
  const T* operator->() const;
  T* operator->();

  Object* __getPtr() const;
  static bool __is(uint64_t raw);
  static ObjectPtr<T> __remember(uint64_t raw);
  uint64_t __forget();

 private:
  Object* ptr;
};

#include "object.h"
