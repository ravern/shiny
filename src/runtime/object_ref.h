#pragma once

#include <cstdint>

class Object;
struct ObjectRefInner;

class ObjectRef {
 public:
  ObjectRef(Object&& object);
  ObjectRef(const ObjectRef& other);
  ObjectRef(ObjectRef&& other);
  ~ObjectRef();

 public:
  ObjectRef& operator=(const ObjectRef& other);
  ObjectRef& operator=(ObjectRef&& other);
  Object& operator*() const;
  Object* operator->() const;

 public:
  Object* get() const;
  uint64_t getCount() const;

 private:
  ObjectRefInner* inner;
};