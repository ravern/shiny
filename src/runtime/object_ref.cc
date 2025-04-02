#include "object_ref.h"

#include "object_ref_inner.h"

ObjectRef::ObjectRef(Object&& object) : inner(new ObjectRefInner{object, 1}) {}

ObjectRef::ObjectRef(const ObjectRef& other) : inner(other.inner) {
  inner = other.inner;
  if (inner != nullptr) {
    ++inner->refCount;
  }
}

ObjectRef::ObjectRef(ObjectRef&& other) : inner(other.inner) {
  inner = other.inner;
  other.inner = nullptr;
}

ObjectRef::~ObjectRef() {
  if (inner) {
    --inner->refCount;
    if (inner->refCount == 0) {
      delete inner;
    }
  }
}

ObjectRef& ObjectRef::operator=(const ObjectRef& other) {
  inner = other.inner;
  if (inner != nullptr) {
    ++inner->refCount;
  }
  return *this;
}

ObjectRef& ObjectRef::operator=(ObjectRef&& other) {
  inner = other.inner;
  other.inner = nullptr;
  return *this;
}

Object& ObjectRef::operator*() const { return inner->object; }
Object* ObjectRef::operator->() const { return &inner->object; }

Object* ObjectRef::get() const { return &inner->object; }
uint64_t ObjectRef::getCount() const { return inner->refCount; }