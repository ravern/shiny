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

  ObjectPtr& operator=(const ObjectPtr& other);
  ObjectPtr& operator=(ObjectPtr&& other);
  T* operator->();

  static ObjectPtr<T> __remember(uint64_t raw);
  uint64_t __forget();

  Object* getPtr() const { return ptr; }

 private:
  Object* ptr;
};

#include "object.h"
