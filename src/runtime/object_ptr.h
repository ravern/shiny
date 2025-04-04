#pragma once

#include <utility>

#include "object.h"

class Object;

template <typename T>
class ObjectPtr {
 public:
  ObjectPtr(T&& object) : object(new Object(std::move(object))) {}

 public:
  ObjectPtr(const ObjectPtr& other);
  ObjectPtr(ObjectPtr&& other);
  ~ObjectPtr();

 public:
  ObjectPtr& operator=(const ObjectPtr& other);
  ObjectPtr& operator=(ObjectPtr&& other);

 public:
  template <typename U>
  bool is() const;
  const T& get() const;
  T& get();

 private:
  Object* object;
};